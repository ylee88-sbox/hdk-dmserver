#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <os_functions.h>
//#include <turbo/dmsT_manager.h>

void * dmsLibID;
char dmsBuf[BUFSIZE], idmbCmdBuffer[BUFSIZE], dmsWorkingDir[BUFSIZE];
char theToolName[BUFSIZE];
int idmbOneLineCommandMode, idmbGraphicMode;
FILE* fptr=NULL;
char command[BUFSIZE];
void idmbExecuteCommand(char *);
int count=0;
void fwidmb_setup(void)
{
  putenv("DEBUGDMS=1");
  dmsCPIInitDMS("idmb","idmb",TRUE);
}

void fwidmb_setup_2(void)
{
  putenv("DEBUGDMS=1");
  dmsCPIInitDMS("idmb","idmb",TRUE);
}
 
void fwidmb_teardown(void)
{
  unsetenv("DEBUGDMS");
}

/**************************************
Utilities function
//tmpLibName,theConfName,cName,vName,verName)
**************************************/
void insert_command(char *input, char *toolName, char *libName,char *confName,char *cellName,char *viewName,char *verName,char *owner, char *user, char * stringVal){
   FILE *ft; // creates a pointer to a file
   ft = fopen("/tmp/logfile.txt","w"); 
 
   if(ft !=0){
      fputs(input,ft);
      fputc('\n',ft);

      if(toolName!=""){
      fputs(toolName,ft);
      fputc('\n',ft);
      }

      if(libName!=""){
      fputs(libName,ft);
      fputc('\n',ft);
      }

      if(confName!=""){
      fputs(confName,ft);
      fputc('\n',ft);
      }

      if(cellName!=""){
      fputs(cellName,ft);
      fputc('\n',ft);
      }

      if(viewName!=""){
      fputs(viewName,ft);
      fputc('\n',ft);
      }

      if(verName!=""){
      fputs(verName,ft);
      fputc('\n',ft);
      }

      if(owner!=""){
        fputs(owner,ft);
        fputc('\n',ft);
      }
      if(user!=""){
         fputs(user,ft);
         fputc('\n',ft);
      }

      if(stringVal!=""){
         fputs(stringVal,ft);
         fputc('\n',ft);
      }

      fputc('\n',ft);
      fclose(ft);
   } else {
      printf("canot open temp file \n");
      exit (1);
   }
}

/**************************************************************
TEST CASE  : test_lowercase_q_command
DESCRIPTION: Test command able to accept lowercase 
***************************************************************/
START_TEST(test_lowercase_q_command)
{
//putenv("TURBO_DEBUG=1"); 
 //unsetenv("DEBUGDMS");
    fptr = stdout;
    insert_command("q","","","","","","","","","");
    freopen ( "/tmp/logfile.txt", "r",stdin );
    idmbExecuteCommand(command);
}
END_TEST

/**************************************************************
TEST CASE  : test_uppercase_q_command
DESCRIPTION: Test command able to accept uppercase 
***************************************************************/
START_TEST(test_uppercase_q_command)
{
   fptr = stdout;
   insert_command("Q","","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

/**************************************************************
TEST CASE  : test_c_command
DESCRIPTION: Cancel Checkout 
***************************************************************/
START_TEST(test_c_command)
{
printf("test_c_command\n");
   char *wa =  getenv("WORK");  
   char path[100];
   strcpy( path, wa );
   strcat( path, "/genesys/lnf/test.lnf" );
   struct stat sb;

   fptr = stdout;
   insert_command("c","genesys","utlib","test","lnf","current","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
  
   // Make sure test.lnf get deleted 
   ck_assert(!(stat(path, &sb) == 0 && S_ISREG(sb.st_mode)));
}
END_TEST


/**************************************************************
TEST CASE  : test_abort_command
DESCRIPTION: Test command to abort 
***************************************************************/
START_TEST(test_abort_command)
{
   fptr = stdout;
   insert_command(";","","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_dollar_command)
{
   fptr = stdout;
   insert_command("$","","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_percentage_command)
{
   fptr = stdout;
   insert_command("%","","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_tilt_command)
{
   fptr = stdout;
   insert_command("^","","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_v_command)
{
   fptr = stdout;
   insert_command("v","genesys","utlib","test","lnf","current","*","*","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST


START_TEST(test_m_m_command)
{
   fptr = stdout;
   insert_command("m","m","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_m_u_command)
{
   fptr = stdout;
   insert_command("m","u","hello","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_m_e_command)
{
   fptr = stdout;

   //If DMSPATH is available
   insert_command("m","e","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_p_l_command)
{
   fptr = stdout;
   insert_command("p","l","genesys","utlib","cw_16to1anamux","lnf","current","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_r_command)
{
   fptr = stdout;
   insert_command("p","r","genesys","utlib","test","lnf","current","prop","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_c_lib_asterik_s_command)
{
   fptr = stdout;
   insert_command("p","c","genesys","*","test","lnf","current","pName","s","string");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_c_lib_asterik_i_command)
{
   fptr = stdout;
   insert_command("p","c","genesys","*","test","lnf","current","pName","i","8");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_c_lib_asterik_b_command)
{
   fptr = stdout;
   insert_command("p","c","genesys","*","test","lnf","current","pName","b","t");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_c_lib_asterik_t_command)
{
   fptr = stdout;
   insert_command("p","c","genesys","*","test","lnf","current","pName","t","Fri Jul 15 02:39:18 2016");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_c_lib_asterik_t_command_2)
{
   fptr = stdout;
   insert_command("p","c","genesys","*","test","*","current","pName","t","Fri Jul 15 02:39:18 2016");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_p_c_lib_asterik_def_command)
{
   fptr = stdout;
   insert_command("p","c","genesys","*","test","lnf","current","pName","def","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_y_l_command)
{
   fptr = stdout;
   insert_command("y","l","genesys","utlib","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_c_command)
{
   fptr = stdout;
   insert_command("y","c","genesys","utlib","default","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST


START_TEST(test_y_t_no_command)
{
   fptr = stdout;
   insert_command("y","t","genesys","utlib","default","hello","no","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_t_yes_command)
{
   fptr = stdout;
   insert_command("y","t","genesys","utlib","default","hello","y","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_o_command)
{
   fptr = stdout;
   insert_command("y","o","genesys","utlib","test","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_d_command)
{
   fptr = stdout;
   insert_command("y","d","genesys","utlib","default","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
   
}
END_TEST

START_TEST(test_y_s_command)
{
   fptr = stdout;
   insert_command("y","s","genesys","utlib","default","d","default","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_v_command)
{
   fptr = stdout;
   insert_command("y","v","genesys","utlib","default","utlib","default","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_i_command)
{
   fptr = stdout;
   insert_command("y","i","genesys","utlib","default","dd","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_m_command)
{
   fptr = stdout;
   insert_command("y","m","genesys","utlib","default","unit_testing","no","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_a_s_command)
{
   fptr = stdout;
   insert_command("y","a","genesys","utlib","default","test","lnf","current","s","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST


START_TEST(test_y_a_t_command)
{
   fptr = stdout;
   insert_command("y","a","genesys","utlib","default","test","lnf","current","t","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST


START_TEST(test_y_a_p_command)
{
   fptr = stdout;
   insert_command("y","a","genesys","utlib","default","test","lnf","current","p","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_r_command)
{
   fptr = stdout;
   insert_command("y","r","genesys","utlib","default","test","lnf","current","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_f_command)
{
   fptr = stdout;
   insert_command("y","f","genesys","utlib","default","utlib","default","a","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_n_command)
{
   fptr = stdout;
   insert_command("y","n","genesys","utlib","default","d","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_y_x_command)
{
   fptr = stdout;
   insert_command("y","x","genesys","utlib","default","d","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST


START_TEST(test_i_command)
{
   char* work_dir = getenv("WORK_AREA_ROOT_DIR");
   char* installed_dir;

   installed_dir = malloc(strlen(work_dir)+20);
   strcpy(installed_dir,work_dir); /* copy name into the new var */
   strcat(installed_dir, "/genesys/lnf"); /* add the extension */

   fptr = stdout;
   FILE *cell_file;
   struct stat stc;
   char *cfile  = "/tmp/ut_test_i_command.txt";
   char buffer[100];

   char *cell_dir;
   cell_dir = malloc(strlen(installed_dir)+20);
   strcpy(cell_dir,installed_dir); /* copy name into the new var */
   strcat(cell_dir, "/test2.lnf"); /* add the extension */

   //1 CAN NOT CHECKIN/UPDATE/INSTALL READONLY COPY 
   snprintf(buffer, sizeof(buffer), "touch %s", cell_dir);
   system(buffer);
   cell_file = fopen("/tmp/ut_test_i_command.txt","w");

   fputs("test2.lnf",cell_file);
   fputc('\n',cell_file);
   fclose(cell_file);
   snprintf(buffer, sizeof(buffer), "chmod 444  %s", cell_dir);
   system(buffer);

   insert_command("i","genesys","utlib","/tmp/ut_test_i_command.txt","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

   //2. Success import
   snprintf(buffer, sizeof(buffer), "chmod 777 %s", cell_dir);
   system(buffer);
  
   insert_command("i","genesys","utlib","/tmp/ut_test_i_command.txt","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

   //3.ALREADY EXISTS IN LIBRARY 
   snprintf(buffer, sizeof(buffer), "touch %s", cell_dir);
   system(buffer);

   snprintf(buffer, sizeof(buffer), "chmod 777 %s", cell_dir);
   system(buffer);

   insert_command("i","genesys","utlib","/tmp/ut_test_i_command.txt","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
   
   // clean up
   snprintf(buffer, sizeof(buffer), "rm -rf %s", cell_dir);
   system(buffer);
//4. Cannot open file
   insert_command("i","genesys","utlib","/tmp/ut_test_fake.txt","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

   //5. CAN NOT OPEN LIBRARY
   insert_command("i","genesys","fakelib","/tmp/ut_test_i_command.txt","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
 
    //6. NO FILE
    cell_file = fopen("/tmp/ut_test_i_command.txt","w");
    fputs("vvvvv.lnf",cell_file);
    fputc('\n',cell_file);
    fclose(cell_file);

   insert_command("i","genesys","utlib","/tmp/ut_test_i_command.txt","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_x_command_no_export_dir)
{
   printf("test_x_command_no_export_dir\n");
   struct stat st;
   char *exp_file  = "/tmp/exp_dir";
   if (stat(exp_file, &st) == 0) {
     rmdir(exp_file);
   }

   fptr = stdout;

   insert_command("x","genesys","utlib","/tmp/ut_test_x_command.txt",exp_file,"","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

void create_cellFile(){

   // Create dummy file
   FILE *cell_file;

   struct stat stc;
   char *cfile  = "/tmp/ut_test_x_command.txt";
//   if (stat(cfile, &stc) == -1 ) {
     cell_file = fopen("/tmp/ut_test_x_command.txt","w");
     fputs("test.lnf",cell_file);
     fputc('\n',cell_file);
     fclose(cell_file);
  // }
}

// Write permission denied
START_TEST(test_x_command_dir_no_permission)
{
   // Create directory with no write permission
   struct stat st;
   char *exp_file  = "/tmp/exp_dir";
   if (stat(exp_file, &st) == -1) {
     mkdir(exp_file, 0000);
   }

   fptr = stdout;
   insert_command("x","genesys","utlib","/tmp/ut_test_x_command.txt",exp_file,"","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST


START_TEST(test_x_command)
{
   struct stat st;
   char *exp_file  = "/tmp/exp_dir1";
   if (stat(exp_file, &st) == -1) {
     mkdir(exp_file,0777);
   }

   fptr = stdout;
   insert_command("x","genesys","utlib","/tmp/ut_test_x_command.txt",exp_file,"","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

//CAN NOT FIND FILE: v42.lnf
START_TEST(test_x_command_invalid_cellfile)
{
   struct stat st;
   char *exp_file  = "/tmp/exp_dir1";
   if (stat(exp_file, &st) == -1) {
     mkdir(exp_file,0777);
   }

   // Create dummy file
   FILE *cell_file2;

   struct stat stc;
   char *cfile  = "/tmp/ut_test_x_command2.txt";
   if (stat(cfile, &stc) == -1 ) {
     cell_file2 = fopen("/tmp/ut_test_x_command2.txt","w");
     fputs("v42.lnf",cell_file2);
     fputc('\n',cell_file2);
     fclose(cell_file2);
   }

   fptr = stdout;
   insert_command("x","genesys","utlib","/tmp/ut_test_x_command2.txt",exp_file,"","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

 // unlink("/tmp/ut_test_x_command2.txt");

}
END_TEST

//CAN NOT OPEN LIBRARY: f FOR READ
START_TEST(test_x_invalid_lib_command)
{
   fptr = stdout;
   insert_command("x","genesys","f","c",".","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

//Cannot open file c
START_TEST(test_x_invalid_cell_command)
{
   fptr = stdout;
   insert_command("x","genesys","utlib","c",".","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_s_s_command)
{
   fptr = stdout;
   insert_command("s","s","genesys","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_s_p_command)
{
   fptr = stdout;
   insert_command("s","p","genesys","lnf","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST


START_TEST(test_s_r_command)
{
   fptr = stdout;
   insert_command("s","r","","","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_s_w_command)
{
   fptr = stdout;
   insert_command("s","w","genesys","lnf","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_s_l_command)
{
   fptr = stdout;
   insert_command("s","l","genesys","lnf","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

START_TEST(test_g_command)
{
   fptr = stdout;
   insert_command("g","genesys","utlib","default","test","lnf","current","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_l_command)
{
printf("test_l_command\n");
   char *wa =  getenv("WORK");  
   char path[100];
   strcpy( path, wa );
   strcat( path, "/genesys/lnf/test.lnf" );
   struct stat sb; 

   fptr = stdout;
   insert_command("l","genesys","utlib","default","test","lnf","current","","","");
   //insert_command("l","genesys","nonprdpng_refhdk_lay","default","v5","lnf","current","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

   // Make sure test.lnf exist in work dir and is in read write mode 
   ck_assert(osFileIsRW(path));
   ck_assert((stat(path, &sb) == 0 && S_ISREG(sb.st_mode)));
}
END_TEST

START_TEST(test_e_lib_asterik_command)
{
   fptr = stdout;
   insert_command("e","genesys","*","test","lnf","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_e_lib_specific_command)
{
   fptr = stdout;
   insert_command("e","genesys","utlib","test","lnf","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_f_command)
{
  fptr = stdout;
  char buffer[100];
  char *wa =  getenv("WORK");  
  char path[100];
  //struct stat sb;
  strcpy( path, wa );
  strcat( path, "/genesys/lnf/test.lnf" );

  insert_command("f","genesys","utlib","default","test","lnf","current","","","");
  freopen ( "/tmp/logfile.txt", "r",stdin );
  idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_f_command_remove_ro_file)
{
  fptr = stdout;
  char buffer[100];
  char *wa =  getenv("WORK");  
  char path[100];
  //struct stat sb;
  strcpy( path, wa );
  strcat( path, "/genesys/lnf/test.lnf" );

  insert_command("f","genesys","utlib","default","test","lnf","current","","","");
  freopen ( "/tmp/logfile.txt", "r",stdin );
  idmbExecuteCommand(command);

  // Verify file is read only
  ck_assert(osFileIsRO(path)); 
  snprintf(buffer, sizeof(buffer), "rm -rf %s", path); 
  system(buffer);
}
END_TEST

START_TEST(test_w_command)
{
   // Will not able to test checkin the code as need synch enabled.
printf("test_w_command\n");
   FILE *fs;
   char *wa =  getenv("WORK");  
   char path[100];
   char buffer[100];
   strcpy( path, wa );
   strcat( path, "/genesys/lnf/test.lnf" );
   fs = fopen(path,"w");
   
   if (fs !=0) {
     fputs("edit this file",fs);
     fputc('\n',fs);
     fclose(fs);
   } else {
     printf("canot open %s\n",path);
   }
   sleep(2);
   snprintf(buffer, sizeof(buffer), "chmod 777 %s ", path);
   system(buffer);
   snprintf(buffer, sizeof(buffer), "cp /nfs/png/disks/credo1/mphoon/src/dm_new/tests/utils/test.lnf %s ", path);
   system(buffer);
   snprintf(buffer, sizeof(buffer), "chmod 777 %s ", path);
   system(buffer);
 
   fptr = stdout;
   insert_command("w","genesys","utlib","test","lnf","current","Checkin from idmb.","NONE","no","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_z_command)
{
   fptr = stdout;
   insert_command("z","genesys","utlib","test","lnf","1.2","1.1","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

/*
  Check in without modify any content
*/

START_TEST(test_w_fail_command)
{
printf("test_w_fail_command\n");
   fptr = stdout;
   insert_command("w","genesys","utlib","test","lnf","current","message","NONE","Y","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_r_command)
{
   fptr = stdout;
   insert_command("r","genesys","utlib","test","lnf","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

/*
Install file which is not exist
*/
START_TEST(test_n_fail_command)
{
   fptr = stdout;
   insert_command("n","genesys","test","lnf","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_n_command)
{
   // Create empty file
printf("test_n_command\n");
   char *wa =  getenv("WORK");  
   char path[100];
   char buffer[100];
   strcpy( path, wa );
   strcat( path, "/genesys/lnf/test.lnf" );

 struct stat sb;
  
  if (stat(path, &sb) == 0 && S_ISREG(sb.st_mode)){
    snprintf(buffer, sizeof(buffer), "rm -rf %s ", path);
    system(buffer);  
  }
   snprintf(buffer, sizeof(buffer), "touch %s ", path);
   system(buffer); 
   snprintf(buffer, sizeof(buffer), "chmod 777 %s ", path);
   system(buffer);  
   fptr = stdout;
   insert_command("n","genesys","test","lnf","","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);
}
END_TEST

START_TEST(test_d_command)
{
   fptr = stdout;

   //NO MATCHED CELLVIEW
   insert_command("d","genesys","utlib","ss","lnf","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

   //Match cellview
   insert_command("d","genesys","utlib","test","lnf","","","","","");
   freopen ( "/tmp/logfile.txt", "r",stdin );
   idmbExecuteCommand(command);

}
END_TEST

Suite * fwidmb_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_x_command;
    TCase *tc_m_command;
    TCase *tc_s_command;
    TCase *tc_y_command;
    TCase *tc_p_command;
    TCase *tc_e_command;
    TCase *tc_no_init_idms;
    TCase *tc_q_command;
  
    s = suite_create("fwidmb_suite");
    /* Core test case */
    tc_core = tcase_create("Core");
    tc_x_command = tcase_create("x_command");
    tc_m_command = tcase_create("m_command");
    tc_s_command = tcase_create("s_command");
    tc_y_command = tcase_create("y_command");
    tc_p_command = tcase_create("p_command");
    tc_e_command = tcase_create("e_command");
    tc_no_init_idms = tcase_create("no_init_idms"); 
    tc_q_command = tcase_create("q_command");
  
    tcase_add_checked_fixture(tc_core, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_x_command, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_m_command, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_s_command, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_y_command, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_p_command, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_e_command, fwidmb_setup, fwidmb_teardown);
    tcase_add_checked_fixture(tc_q_command, fwidmb_setup_2, fwidmb_teardown);
    tcase_add_checked_fixture(tc_no_init_idms, fwidmb_setup_2, fwidmb_teardown);
    
    tcase_add_test(tc_core, test_abort_command);
    tcase_add_test(tc_core, test_dollar_command);
    tcase_add_test(tc_core, test_percentage_command);
    tcase_add_test(tc_core, test_tilt_command);  
    tcase_add_test(tc_core, test_i_command);
    tcase_add_test(tc_core, test_n_fail_command);
    tcase_add_test(tc_core, test_n_command); // Install test.lnf
    tcase_add_test(tc_core, test_l_command); // Checkout test.lnf
    tcase_add_test(tc_core, test_w_command); // Checkin test.lnf without modify content
    tcase_add_test(tc_core, test_l_command); // Checkout again test.lnf
    tcase_add_test(tc_core, test_c_command); // Cancel Checkout test.lnf
    tcase_add_test(tc_core, test_l_command);
    tcase_add_test(tc_core, test_w_fail_command); 
    tcase_add_test(tc_core, test_f_command);
    tcase_add_test(tc_core, test_f_command_remove_ro_file);
    tcase_add_test(tc_core, test_v_command);
    tcase_add_test(tc_core, test_d_command);
    tcase_add_test(tc_core, test_g_command); // Read lock
    tcase_add_test(tc_core, test_c_command); // Cancel Checkout test.lnf
    tcase_add_test(tc_core, test_r_command); // Log history of files
    tcase_add_test(tc_core, test_z_command); // Make diff between two versions

    create_cellFile(); // Create cell file list to export    
    tcase_add_test(tc_x_command, test_x_command_no_export_dir);
    tcase_add_test(tc_x_command, test_x_command_dir_no_permission);
    tcase_add_test(tc_x_command, test_x_invalid_lib_command);
    tcase_add_test(tc_x_command, test_x_invalid_cell_command);   
    tcase_add_test(tc_x_command, test_x_command);
    tcase_add_test(tc_x_command, test_x_command_invalid_cellfile);

    tcase_add_test(tc_m_command, test_m_m_command);
    tcase_add_test(tc_m_command, test_m_u_command);
    tcase_add_test(tc_m_command, test_m_e_command);

    tcase_add_test(tc_s_command, test_s_s_command);
    tcase_add_test(tc_s_command, test_s_p_command);
    tcase_add_test(tc_s_command, test_s_r_command);
    tcase_add_test(tc_s_command, test_s_w_command);
    tcase_add_test(tc_s_command, test_s_l_command);

    tcase_add_test(tc_y_command, test_y_a_s_command);
    tcase_add_test(tc_y_command, test_y_a_t_command);
    tcase_add_test(tc_y_command, test_y_a_p_command);
    tcase_add_test(tc_y_command, test_y_r_command);
    tcase_add_test(tc_y_command, test_y_f_command);
    tcase_add_test(tc_y_command, test_y_l_command);
    tcase_add_test(tc_y_command, test_y_c_command);
    tcase_add_test(tc_y_command, test_y_t_no_command);
    tcase_add_test(tc_y_command, test_y_t_yes_command);
    tcase_add_test(tc_y_command, test_y_o_command);
    tcase_add_test(tc_y_command, test_y_d_command);
    tcase_add_test(tc_y_command, test_y_s_command);
    tcase_add_test(tc_y_command, test_y_v_command);
    tcase_add_test(tc_y_command, test_y_i_command);
    tcase_add_test(tc_y_command, test_y_m_command);
    tcase_add_test(tc_y_command, test_y_n_command);
    tcase_add_test(tc_y_command, test_y_x_command);
    
    tcase_add_test(tc_p_command,test_p_l_command);
    tcase_add_test(tc_p_command,test_p_r_command);
    tcase_add_test(tc_p_command,test_p_c_lib_asterik_s_command);
    tcase_add_test(tc_p_command,test_p_c_lib_asterik_i_command);
    tcase_add_test(tc_p_command,test_p_c_lib_asterik_b_command);
    tcase_add_test(tc_p_command,test_p_c_lib_asterik_t_command);
    tcase_add_test(tc_p_command,test_p_c_lib_asterik_t_command_2);
    tcase_add_test(tc_p_command,test_p_c_lib_asterik_def_command);
   
    tcase_add_test(tc_e_command, test_e_lib_asterik_command);
    tcase_add_test(tc_e_command, test_e_lib_specific_command);
   
    tcase_add_test(tc_q_command, test_lowercase_q_command); //quit
    tcase_add_test(tc_q_command, test_uppercase_q_command); //quit

    tcase_add_test(tc_no_init_idms,test_c_command);  

    suite_add_tcase(s, tc_core);    
    suite_add_tcase(s, tc_x_command);
    suite_add_tcase(s, tc_m_command);
    suite_add_tcase(s, tc_s_command);
    suite_add_tcase(s, tc_y_command);
    suite_add_tcase(s, tc_p_command);
    suite_add_tcase(s, tc_e_command);
    suite_add_tcase(s, tc_no_init_idms);
    //suite_add_tcase(s, tc_q_command);
    return s;
}
