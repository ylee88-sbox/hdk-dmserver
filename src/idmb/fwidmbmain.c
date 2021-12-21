#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/** fwd decls **/
#ifdef __cplusplus
extern "C" {
#endif
#define IDMB_MAIN
#include <idmb_variables.h> 
#undef IDMB_MAIN
#include <dmsC_new_vars.h>
#include <dmsC.h>
#include <dmsC_new.h>
void idmbReportMsgIfAny(const char *, char *);
int dmsReadInput(char *,char *,char *);
void idmbMsgFromDFII(void);
void idmbExecuteCommand(char *);
#ifdef __cplusplus
}
#endif

int idmbOneLineCommandMode;
char theToolName[BUFSIZE], idmbCmdBuffer[BUFSIZE];
FILE*fptr = NULL;

int main(int argc,char *argv[])
{
  int i;

  char *p=0,*pp=0,dmsBootName[BUFSIZE],command[BUFSIZE],tmpString[BUFSIZE];
  fptr = stdout;
  messageHeader=TRUE;

  strcpy(dmsBootName,argv[0]);

  strcpy(tmpString,argv[0]);
  p=strtok(tmpString,"/");
  while (p) {
    pp=p;
    p=strtok('\0',"/");
  }
  strcpy(theToolName,pp);


  for (i=1;i<argc;i++) if (!strcmp(argv[i],"-one")) {
      idmbOneLineCommandMode=TRUE;
      goto ExecuteProgram;
  }

  for (i=1;i<argc;i++) if (!strcmp(argv[i],"-nullgt")) {
      fptr = stderr; 
      goto ExecuteProgram;
  }

  idmbGraphicMode=TRUE; /* actually Skill ipc mode */

 ExecuteProgram:
  
  dmsCPIInitDMS("idmb",argv[0],TRUE);

  if (idmbGraphicMode)
    idmbMsgFromDFII();
  else {
  idmbReportMsgIfAny(dmsCGetLastMessage(),"");
  while (command[0]!='Q') {

      fprintf(STDPORT,"\nintel Database Management Browser [%s]\n",DMSVERSION);
      fprintf(STDPORT,"[S] DMS.PATH management: Search path/Workdir/Worklib/Reparse/Print result\n");
      fprintf(STDPORT,"[Y] Configuration management: List/Create/Text/Load/Delete/Add/Remove/Diff/Export/Insert/SubConfig/Instance/Comment\n");
      fprintf(STDPORT,"[A] Audittrail management: Enable/Disable/Inquire\n");
      fprintf(STDPORT,"[P] Property management: Create/List/Remove\n");
      fprintf(STDPORT,"[V] View library database\n");
      fprintf(STDPORT,"[I] Import files into a library\n");
      fprintf(STDPORT,"[X] Export files out of a library\n");
      fprintf(STDPORT,"[L] Checkout files to workdir (co -l)\n");
      fprintf(STDPORT,"[C] Cancel checkout of files (unlock)\n");
      fprintf(STDPORT,"[N] Install a new file in worklib (initci)\n");
      fprintf(STDPORT,"[F] Fetch files to workdir for readonly (co)\n");
      fprintf(STDPORT,"[W] Checkin files from working directory (ci)\n");
      fprintf(STDPORT,"[Z] Make diff between two versions of files (diff)\n");
      fprintf(STDPORT,"[R] Log history of files (rlog)\n");
      fprintf(STDPORT,"[E] Find physical location of files\n");
      fprintf(STDPORT,"[G] Read/Lock files based on policy\n");
      fprintf(STDPORT,"[D] Delete files\n");
      fprintf(STDPORT,"[M] Misc utility: Memory usage/Unix command/Edit dms.pth\n");
      fprintf(STDPORT,"[;] Abort commands\n");
      fprintf(STDPORT,"[Q] Quit iDMB\n");
      
      fflush(STDPORT);

      if (idmbOneLineCommandMode) {
	  idmbOneLineCommandMode=FALSE;
	  dmsReadInput("One Line Command:","",idmbCmdBuffer);
	  idmbOneLineCommandMode=TRUE;
      }

      idmbExecuteCommand(command);
    }
  }
}

