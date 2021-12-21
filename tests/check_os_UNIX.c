#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>
#include <time.h> 
#include <pwd.h>
#include <sys/stat.h>

void os_UNIX_setup(void)
{
}
 
void os_UNIX_teardown(void)
{
}

/**************************************************************
TEST CASE  : test_osGetSystemTime
DESCRIPTION: Test osGetSystemTime() able to gets the time from 
             the system, applies offset and returns value.An 
             offset of '0' (zero) returns the current time. 
***************************************************************/
START_TEST(test_osGetSystemTime_1)
{
  time_t now = osGetSystemTime(0);
  time_t match_time = time(0);
  
  ck_assert_str_eq(ctime(&now),ctime(&match_time));
  
}
END_TEST

/**************************************************************
TEST CASE  : test_osGetSystemTime
DESCRIPTION: Test osGetSystemTime() able to gets the time from 
             the system, applies offset and returns value.
***************************************************************/
START_TEST(test_osGetSystemTime_2)
{
  time_t add_now =2; //add 2 sec on the current time
  time_t latest_time = osGetSystemTime(add_now);
  time_t match_time = time(0)+add_now;

  ck_assert_str_eq(ctime(&latest_time),ctime(&match_time));

}
END_TEST

START_TEST(test_osParseSystemTime)
{
  const char *months[13]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","Pth"};

  char *strings;
  strings = "999 ";

  char* name_with_extension;
  char* name_with_extension2;

  name_with_extension = malloc(strlen(strings));
  name_with_extension2 = malloc(30);

  int i;
  for(i=0;i<13;i++){
    strcpy(name_with_extension, strings); 
    strcat(name_with_extension, months[i]); 
    
    strcpy(name_with_extension2,name_with_extension);
    strcat(name_with_extension2, " 12 01:02:03 2016\n\0"); 
    osParseSystemTime(name_with_extension2); 
  }
}
END_TEST

START_TEST(test_osGetSysMemUsed)
{
  ck_assert_int_ne(osGetSysMemUsed(),0);
  
}
END_TEST


START_TEST(test_osCreateDir)
{
  ck_assert_int_eq(osCreateDir("/tmp/new"),1); 
}
END_TEST

START_TEST(test_osCreateFile)
{
  ck_assert_int_eq(osCreateFile("/tmp/createFile.txt"),1);
}   
END_TEST

START_TEST(test_osFilePermission)
{
  char* path = "/tmp/createFile.txt";
  char* rename = "/tmp/newName.txt";
  char buffer[100];

  struct stat info;

  ck_assert_int_eq(osRenameFile(path,rename),0);
  snprintf(buffer, sizeof(buffer), "chmod 000 %s",rename);
  system(buffer);

  stat(rename, &info);  // Error check omitted
  struct passwd *pw = getpwuid(info.st_uid);
  ck_assert_int_eq(osFileIsReadable(rename),0);
  ck_assert_int_eq(osFileIsExecutable(rename),0);
  ck_assert_str_eq(osGetFileOwner(rename),pw->pw_name);
  ck_assert_int_eq(osFileSize(rename),0);
  ck_assert_int_ge(osGetProcessID(),0);
  ck_assert_int_ge(isRegularFile(rename),1);

  snprintf(buffer, sizeof(buffer), "chmod 777 %s",rename);
  system(buffer);

  ck_assert_int_eq(osCreateSymLink(rename,"/tmp/create_link.txt"),1);

}
END_TEST

START_TEST(test_osSystem)
{
  ck_assert_int_eq(osSystem("echo Hi"),0);
}
END_TEST

START_TEST(test_osCopyString)
{
  ck_assert_str_eq(osCopyString("copyString test"),"copyString test");
}
END_TEST

START_TEST(test_osDisplayInfoMsg)
{
  ck_assert_int_eq(osDisplayInfoMsg("displayInfo"),1);
}
END_TEST

START_TEST(test_warnMsg)
{
  ck_assert_int_eq(osBuildWarnMsg("warn1"),1);
  osDisplayWarnMsg();
  ck_assert_int_le(osDeleteWarnMsg(),20480000); // BUFSZ- 2048
}
END_TEST

START_TEST(test_linux_system)
{
  ck_assert_int_eq(linux_system("echo hi"),0);
}
END_TEST

Suite * os_UNIX_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("os_UNIX_suite");
    tc_core = tcase_create("Core");
   
    tcase_add_checked_fixture(tc_core, os_UNIX_setup, os_UNIX_teardown);

    tcase_add_test(tc_core, test_osGetSystemTime_1);
    tcase_add_test(tc_core, test_osGetSystemTime_2);
    tcase_add_test(tc_core, test_osParseSystemTime);
    tcase_add_test(tc_core, test_osGetSysMemUsed);
    tcase_add_test(tc_core, test_osCreateDir);
    tcase_add_test(tc_core, test_osCreateFile);
    tcase_add_test(tc_core, test_osFilePermission);
    tcase_add_test(tc_core, test_osSystem);
    tcase_add_test(tc_core, test_osCopyString);
    tcase_add_test(tc_core, test_osDisplayInfoMsg);
    tcase_add_test(tc_core, test_warnMsg);
    tcase_add_test(tc_core, test_linux_system);

    suite_add_tcase(s, tc_core);
    return s;
}
