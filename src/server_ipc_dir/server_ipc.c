#include <netdb.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <p2pitc/ntsock.h>
#include <p2pitc/interp2p.h>
#include <p2pitc/opusp2p.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#ifndef WIN32
#include <sys/ioctl.h>
#endif
#include <p2pitc/ntsock.h>
#include <p2pitc/timer.h>
#include <p2pitc/log.h>



/* Global variables */
static SOCKET       lsock=0;              /* listen socket */
static SOCKET       csock=0;
static int server_verbose;
static FILE * logFile=NULL;
static char *  fileName=NULL;

static char buffer[32000];
struct timeval gtimeout;


static int waitForDataOrKey(SOCKET csock,int* dataRead)
{
  fd_set read_fds;
  fd_set test_fds;
  struct termios old_term, new_term;
  struct stat buf;
  struct sockaddr addr;
  
  int res=1;
  int xFc=32;
  int t=0;
  int ss=sizeof(addr);
  int m;
  int selRes=0;
  struct sigaction act, oact;
  
  
  FD_ZERO(&read_fds);       /* Clear the fd_set */
  
  FD_SET(csock, &read_fds);
  /*FD_SET(0, &read_fds);*/
  
 
/*  tcgetattr(fileno(stdin), &old_term);
   new_term = old_term; */
  
  /* change terminal settings */
  new_term.c_lflag &= ~(ICANON | ECHO); /* in raw mode */
  new_term.c_cc[VMIN] = '\01';    /* read one char a time */
  new_term.c_cc[VTIME] = '\00';   /* wait time for reading */
  
  /* set terminal in new state */
  //tcsetattr(fileno(stdin), TCSANOW, &new_term);
  fflush(stdout);
  memset(&act,0,sizeof(act));
  memset (&oact,0,sizeof(act));
  sigemptyset(&act.sa_mask);
  act.sa_handler = SIG_IGN;

  sigaction(SIGALRM, &act, &oact);
    
  while (1){
    errno=0;
    selRes=select(csock+1,&read_fds, NULL, NULL, &gtimeout);;
    if (selRes==SOCKET_ERROR){
      if (errno!=EINTR){
        res=0;
        break;
      } else {
        continue;
      }
    } else {
      if (!selRes) { //timeOut reached no response.
        res=0;
      }
    }
    break;
  }
  sigaction(SIGALRM, &oact, &act);  
  
 
  //for (t=0;t<xFc;t++) {
  //  if (FD_ISSET(t,&read_fds))
  //    printf( "FD %d  1\n",t);
  //}
  //fflush(stdout);
  *dataRead = -1;
  if (res) {
    if (FD_ISSET(0,&read_fds)) {
      *dataRead=getchar();
      fseek(stdin,-1,1);
      //printf("character pressed %c\n",*dataRead);
    }
    //tcsetattr(fileno(stdin), TCSANOW, &old_term);
    return FD_ISSET(csock,&read_fds) ? csock : 0;
  }
  //tcsetattr(fileno(stdin), TCSANOW, &old_term);
  return -1;
}

static int getMsg( SOCKET csock, int timeOut)
{
  
  fd_set read_fds;
  
  FD_ZERO(&read_fds);		/* Clear the fd_set */
  FD_SET(csock, &read_fds);
  FD_SET(0, &read_fds);
  
  if ( select(csock+1,  &read_fds, NULL, NULL, NULL) == SOCKET_ERROR) {
    return -1;
  }
  
  return FD_ISSET(csock,&read_fds) ? csock : 0;
  
}

static
int writeLine ( SOCKET csock, const char * line)
{
  int len=0;
  if (CheckForWrite(csock)) {
    len=send(csock, line, strlen(line)+1,0);
  }
  return len == strlen(line)+1;
}
int  settimeout_ipc(int seconds)
{
  gtimeout.tv_sec=seconds;
}

int  gettimeout_ipc( )
{
  return gtimeout.tv_sec;
}

 
static
int readLine ( SOCKET csock,  char * line, int msize)
{
  int len=0;
  int ioctlCmd = FIONREAD;
  int ioctlArg = 0;
  int toverval = 0;
  char*tover;
  struct sigaction act, oact;
  fd_set read_fds;
  
  FD_ZERO(&read_fds);			/* Clear the fd_set */
  FD_SET(csock, &read_fds);

  memset(&act,0,sizeof(act));
  memset (&oact,0,sizeof(act));
  sigemptyset(&act.sa_mask);
  act.sa_handler = SIG_IGN;
  sigaction(SIGALRM, &act, &oact);

  while (1){
    errno=0;
    if ( select(csock+1,&read_fds, NULL, NULL, &gtimeout) == SOCKET_ERROR) {
      if (errno!=EINTR){
	ErrorLog(EXIT, WinSockError());
	break;
      } else {
	continue;
      }
    }
    break;
  }
 
  if (! FD_ISSET(csock, &read_fds)) {
    sigaction(SIGALRM, &oact, &act);
    return (0);
  } 
  
  if ( ioctlsocket(csock,ioctlCmd, &ioctlArg) == SOCKET_ERROR) {
    sigaction(SIGALRM, &oact, &act);
    return 0;
  }
  
  if(msize){
  	ioctlArg = msize;
  }
  len=recv(csock,line,ioctlArg,0);
  sigaction(SIGALRM, &oact, &act);
  return len&&(len == ioctlArg);
}

static
void  terminateHandler(int signal)
{
  if (signal) {
    fprintf(logFile,"Process terminated by signal %d\n",signal);
  } else {
    fprintf(logFile,"Process terminated normally\n",signal);
  }
  
  
  fflush(logFile);
  fclose (logFile);

  close(csock);
  close(lsock);
  if (!server_verbose) {
    unlink(fileName);
  }
  
}
static
void  pipeHandler(int signal)
{
  close(csock);
  csock=0;
}


static
int processSystemMessages(char * msgStr)
{
  if (strstr(msgStr,"TerminateConnection")){
    close(csock);
    return 0;
  }
  if (strstr(msgStr,"TerminateServer")){
    terminateHandler(0);
    exit(1);
  }
  if (strstr(msgStr,"UnSetConnectionVerbose")){
    server_verbose=0;
    return 3;
  }

  if (strstr(msgStr,"SetConnectionVerbose")){
    server_verbose=1;
    return 2;
  }
  return 1;
  
}

int
isDataOrKey()
{
  int res=-1;
  int key=0;
  
  if (csock){
    res=waitForDataOrKey(csock,&key);
	//Check for error condition and return -1 on error
	if (res == 0 && key == -1){
		return -1;
	}	
  }
  return (res != -1);
}

char*
readLine_ipc()
{
  int res=0;
  int key=0;
  
  if (csock){
    res=waitForDataOrKey(csock,&key);
    if (res==csock){
      res=readLine(csock,buffer,0);
    }
    if (res==-1) {
      return NULL;
    }
    if (res==0) {
      if (key == -1)
        return (char*) -1;
      return (char*)key;
    }
  }
  return res?buffer:NULL;
}


int
evalSkillString_ipc(char *skill)
{
  int res=0;
  
  if (csock) {
    if (server_verbose) {
      fprintf(logFile,"calling EVALSKILL(%s)\n", skill);
      fflush(logFile);
    }
    if (writeLine(csock,skill))
      res=readLine(csock,buffer,1);
    if (server_verbose) {
      fprintf(logFile,"return EVALSKILL(%s)\n", buffer);
      fflush(logFile);
    }
  }
  return res;
}


char*
evalSkillWithReturn_ipc(char * skill)
{
  int res=0;
  
  if (csock) {
    if (server_verbose) {
      fprintf(logFile,"calling EVALSKILLWITHRETURN(%s)\n", skill);
      fflush(logFile);
    }
    strcpy(buffer,"#");
    strcat(buffer,skill);
    if (writeLine(csock,buffer))
      res=readLine(csock,buffer,0);
    if (server_verbose) {
      fprintf(logFile,"return from  EVALSKILLWITHRETURN(%s)\n", buffer);
      fflush(logFile);
    }
  }
  return res ? buffer:NULL;
}


/* ----------------------------------------------------------------- */

static
void evalSkillReturnHandler_sipc(char* msgStr)
{
  int          length;
  
  if (server_verbose) {
    fprintf(logFile,"EVALSKILLWITHRETURN return val is '%s'\n", msgStr);
    fflush(logFile);
  }
  if (processSystemMessages(msgStr))
    writeLine(csock,msgStr);
}

static int GenerateConnection(SOCKET *lsock, int portStart)
{
  SOCKADDR_IN myaddr_in;		/* for local socket address */
  int optlvl = SOL_SOCKET;	/* option setting level */
  int optname = SO_REUSEADDR;	/* option name */
  int optval = 1;			/* setting options for SO_REUSEADDR */
  int optlen = sizeof (optval);	/* sizeof option value */
  int done=0;
  int t;
  
  
  
  
  for (t=0;t<100;t++) {
    memset ((char *)&myaddr_in, 0, sizeof(SOCKADDR_IN));
    myaddr_in.sin_family = AF_INET;
    myaddr_in.sin_addr.s_addr = INADDR_ANY;
    myaddr_in.sin_port = ntohs((short)(portStart+t));
    *lsock = socket (AF_INET, SOCK_STREAM, 0);
    if (*lsock == INVALID_SOCKET) {
      return 0;
    }
    if ( setsockopt(*lsock, optlvl, optname, (char *)&optval, 
                    optlen) == SOCKET_ERROR ) {
      close(*lsock);
      continue;
    }
    if (bind(*lsock, (PSOCKADDR)&myaddr_in, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
      close (*lsock);
      continue;
    }
    if (listen(*lsock, SOMAXCONN) == SOCKET_ERROR) {
      close (*lsock);
      continue;
    }
    done=1;
    
    break;
  }
  if (done ) {
    return portStart+t;
  }
  return 0;
  
}




int connect_ipc()
{
  FILE*portFile=NULL;
  char*serverName=NULL;
  int port=0;
  char * portFileName = getenv("PORT_IPC");
  char*tover=0;
  int toverval=0;
  buffer[0]=0;
  if (portFileName) {
    portFile=fopen(portFileName,"r");
    if (portFile) {
      fgets(buffer,1024,portFile);
      strtok(buffer,"\r\n");
      serverName=strdup(buffer);
      fgets(buffer,1024,portFile);
      port=atoi(buffer);
      strcpy(buffer,serverName);
      free(serverName);
      fclose(portFile);
    }else {
      fprintf(stderr,"Error could not open %s for reading\n",portFileName);
    }
  }else {
    fprintf(stderr,"Error env PORT_IPC  not defined\n");
    
  }
    
  if (serverName && port)
    if (InitiateConnection(buffer,&csock,port)==P2P_SUCCESS){
     gtimeout.tv_sec  = 60;
     gtimeout.tv_usec = 0;
     tover=getenv("TIMEOUT_IPC");
      if(tover){
        toverval=atoi(tover);
        if (toverval){
          gtimeout.tv_sec=atoi(tover);
        }
      }
      return csock;
    } 
 csock=0;
 return 0;
}
int disconnect_ipc()
{
  evalSkillString_ipc("TerminateConnection");
  if (csock){
    close(csock);
  }
  csock=0;
}

#ifdef SERVER_IPC
 
int crsock=0,cwsock=0;
/* ----------------------------------------------------------------- */
int main(int argc, char ** argv)
{
  int rv;
  char handlerNum;
  int port;
  int dataRead;
  char* lf=NULL;
  
  int argcCounter;
  int msgLength;
  
  char logs[256];
  int pid;
  fd_set read_fds;
  fd_set test_fds;
  struct timeval timeout;
  struct stat buf;
 
  port=0;
  
  /* get input arguments if any */
  for(argcCounter=1; argcCounter<=argc-1 ;++argcCounter) {
    if (strcmp(argv[argcCounter],"-trace")==0)
      SetDebugOn();
    else
      if (strcmp(argv[argcCounter],"-read")==0){
        crsock=atoi(argv[++argcCounter]);
        close(0);dup2(crsock,0);
      }
      else
        if (strcmp(argv[argcCounter],"-write")==0){
          cwsock=atoi(argv[++argcCounter]);
          close(1);dup2(cwsock,1);
        }
        else
          if (strcmp(argv[argcCounter],"-port")==0)
            port=atoi(argv[++argcCounter]);
          else
            DebugLog("dmserver: Invalid option");
  }
  
  server_verbose = 0;
  
  if (getenv("SERVER_VERBOSE_IPC")){
    server_verbose = 1;
    SetDebugOn();
  }
  signal(SIGINT,  terminateHandler);
  signal(SIGQUIT, terminateHandler);
  signal(SIGPIPE, pipeHandler);
  if (strstr(argv[0],"client")) {
    if (connect_ipc(argv[2],argv[4],atoi(argv[3]))) {
      for (rv=0;rv<10;rv++) {
        sprintf(buffer,"c=c+%d",rv);
        evalSkillString_ipc(buffer);
        printf( "result of %s %s\n",buffer,evalSkillWithReturn_ipc("c"));
      }
      readLine_ipc();
      
      disconnect_ipc();
      exit(1);
    } else {
      printf("Client error could not connect to %s port %s " ,argv[2],argv[3]);
      exit(1);
    }
  }
  
  
  if (argc >1 ) {
    logFile=fopen(argv[1],"w");
    fileName=strdup(argv[1]);
  } else {
    lf=getenv("PORT_IPC");
    if (lf){
      fileName=strdup(lf);
      logFile=fopen(lf,"w");
    }
    else {
      fprintf(stderr,"Error env. PORT_IPC  not defined\n");
    }
  }
  
  if (!logFile) {
    fprintf(stderr,"Cannot create port file at: %s\n",fileName);
    exit(1);
  }
  
  /* next if should have skill side stderr handler */
  
  /* "select" loop */
  port=GenerateConnection(&lsock,atoi((argc>2) ? argv[2]  : "5001"));
  if (!port) {
    fprintf(logFile,"Cannot find open port to startup server \n");
    exit(1);
  }
  gethostname(buffer,1024);

  fprintf(logFile,"%s\n",buffer);
  fprintf(logFile,"%d\n",port);

  fflush(logFile);
  fclose(logFile);
  logFile=fopen(lf,"w");
    
  if (server_verbose) {
    fprintf(logFile, "After the first wait on Connection\n");
    fflush(logFile);
  }
  while (1) {
    WaitForConnection(&lsock,&csock);

    FD_ZERO(&read_fds);		/* Clear the fd_set */

    FD_SET(csock, &read_fds);
    FD_SET(0, &read_fds);
    
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;
    
    while(1) {
      FD_ZERO(&test_fds);		/* Clear the fd_set */
      FD_SET(0,&test_fds);
      if ((select(64,  &test_fds, 0,0,&timeout) != -1) 
	  && (FD_ISSET(0,&test_fds))) {
        fgets(buffer,1024,stdin);
        if (server_verbose) {
	  fprintf(logFile,"Flushing %s\n",buffer);
	  fflush(logFile);
	}
      } else {
	break;
      }
    }
    
    dataRead=0;
    lseek(csock,0,2);
    
    /* loop forever */
    while (1) {
      if (!csock) {
        break;
      }
      rv = getMsg(csock,0);
      if (rv==-1) {
        if (server_verbose) {
          fprintf(logFile, "getMsg returned -1\n");
          fflush(logFile);
        }
        break;
      }
      
      if (rv==csock) {
        if (server_verbose) {
          fprintf(logFile, "WaitForMsg returns csock=%d\n", csock);
          fflush(logFile);
        }
        dataRead=1;
        
        if (ProcessInput())
          continue;
        else
          break;
      }
      if (server_verbose) {
        if (rv == 0) {
          fprintf(logFile, "WaitForMsg returns 0\n");
          fflush(logFile);
        }
        else if (rv == -1) {
          fprintf(logFile, "WaitForMsg returns -1\n");
          fflush(logFile);
        }
      }
      if (!csock) {
        break;
      }
      
      fgets(buffer,1024,stdin);
      strtok(buffer,"\r\n");
      if (server_verbose) {
        fprintf(logFile,"main loop return val is '%s'\n", buffer);
        fflush(logFile);
      }
      if (dataRead)
        evalSkillReturnHandler_sipc(buffer);
    }
  }
  
  fprintf(stdout, "$\n");
  fflush(stdout);
  if (server_verbose) {
    fprintf(logFile, "server exits from main\n");
    fflush(logFile);
  }else {
    unlink(fileName);
  }
  
}


/* ----------------------------------------------------------------- */
int ProcessInput()
{
  
  int ret=0;
  int sm=0;
  
  ret=readLine(csock,buffer,0);
  if (!ret) {
    return 0;
  }
  
  sm=processSystemMessages(buffer);

  if (sm==1){
    if (buffer[0]!='#'){
      if (evalSkillString_sipc(buffer)){
        ret=writeLine(csock,"");
      }
    }
    else {
      ret=evalSkillWithReturn_sipc(buffer);
    }
  }
  return sm&&ret?1:0;

}

/* ----------------------------------------------------------------- */
/* This one is called from outside (via Mainsail FLI) */
int  evalSkillString_sipc(char *skill)
{
  if (server_verbose) {
    fprintf(logFile,"Inside EVALSKILL(%s)\n", skill);
    fflush(logFile);
  }
  strtok(skill,"\r\n");
  fprintf(stdout, "%s\n", skill);
  fflush(stdout);
  if (server_verbose) {
    fprintf(logFile,"wrote '%s' to stdout\n", skill);
    fflush(logFile);
  }
  
  if (server_verbose) {
    fprintf(logFile,"\n");
    fflush(logFile);
  }
  return 1;
  
}

/* ----------------------------------------------------------------- */
/* This one is called from outside (via Mainsail FLI) */
int evalSkillWithReturn_sipc(char * skill)
{
  if (server_verbose) {
    fprintf(logFile,"Inside EVALSKILLWITHRETURN(%s)\n", skill);
    fflush(logFile);
  }
  strtok(skill,"\r\n\r");
  fprintf(stdout, "%s\n", skill); /* "#" for skill data handler */
  fflush(stdout);
  if (server_verbose) {
    fprintf(logFile,"wrote '#%s' to stdout\n", skill);
    fflush(logFile);
  }
  return(1);
}

#endif





