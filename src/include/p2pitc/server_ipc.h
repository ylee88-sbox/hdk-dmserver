#ifndef SIMP_IPC_H
#define SIMP_IPC_H
int evalSkillString_ipc(char *skill); // returns non zero if successful
char* evalSkillWithReturn_ipc(char * skill); //returns the eval result
int connect_ipc(); // returns non zero if successful
int disconnect_ipc(); // returns non zero if successful
char*  readLine_ipc(); // returns the line send by Opus if available. Null otherwise.
int  settimeout_ipc(int seconds);
int  gettimeout_ipc(); 
int  isDataOrKey();
#endif
