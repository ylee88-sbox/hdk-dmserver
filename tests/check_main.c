
#include <stdlib.h>
#include <check.h>
#include "check_check.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

/*****************************
This is to declare all test suits 
*****************************/

int main() {

    int n;
    SRunner *sr;

    //sr = srunner_create (dmsC_dm_suite());
    sr = srunner_create (fwidmb_suite());
    srunner_add_suite(sr, table_functions_suite());
    srunner_add_suite(sr, C_API_1_suite());
    srunner_add_suite(sr, os_UNIX_suite());
    srunner_add_suite(sr, dmsC_API_new_suite());
    srunner_add_suite(sr, dmsC_functions2_suite());
    srunner_add_suite(sr, dmsC_dm_suite());
    srunner_add_suite(sr, idb_functions_suite());
    srunner_add_suite(sr, dmsC_status_suite());

    srunner_run_all(sr, CK_NORMAL);

    n = srunner_ntests_failed(sr);
    srunner_free(sr);

    cleanup();
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}

void cleanup(){
   clean_file("/tmp/ut_test_i_command.txt");
   clean_file("/tmp/ut_test_x_command2.txt");
   clean_file("/tmp/ut_test_x_command.txt");
   clean_file("/tmp/createFile.txt");
   clean_file("/tmp/create_link.txt");
   clean_file("/tmp/newName.txt");
   clean_dir("/tmp/exp_dir");
   clean_dir("/tmp/exp_dir1");
   clean_dir("/tmp/new");
}

void clean_file(char *filename){

   struct stat st;
   int ret;

   if (stat(filename, &st) == 0) {
     ret = remove(filename);

     if(ret != 0) 
     {
        printf("Error: unable to delete the file %s\n",filename);
     }
   }
}

void clean_dir(char *dirname) {

   char buffer[256];   
   struct stat st;
   int ret;

   if (stat(dirname, &st) == 0) {
      snprintf(buffer, sizeof(buffer), "rm -rf %s", dirname);
      system(buffer);
   }

}
