#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC_new_vars.h>
#include <dmsC_new.h>

void api_1_setup(void)
{
  putenv("DEBUGDMS=1");
  dmsCPIInitDMS("idmb","idmb",TRUE);
}
 
 void api_1_teardown(void)
{

}

/**************************************************************
TEST CASE  : test_coralCDisableObsoleteMessages
DESCRIPTION: Test coralCDisableObsoleteMessages() able to set 
             disable_obsolete_messages to 1
***************************************************************/
START_TEST(test_coralCDisableObsoleteMessages)
{
  ck_assert_int_eq(disable_obsolete_messages,0);
  coralCDisableObsoleteMessages();
  ck_assert_int_eq(disable_obsolete_messages,1);
}
END_TEST

/**************************************************************
TEST CASE  : test_coralCDisableObsoleteMessages
DESCRIPTION: Test coralCDisableObsoleteMessages() able to set 
             disable_obsolete_messages to 1
***************************************************************/
START_TEST(test_dmsCRegisterUserTriggers_1)
{
  // If globalUserTriggerDisableFlag is true will return false
  ck_assert_int_eq(dmsCRegisterUserTriggers(1),0);
  ck_assert_int_eq(dmsCRegisterUserTriggers(0),0);
}
END_TEST


START_TEST(test_dmsCRegisterTriggers)
{
  // If globalUserTriggerDisableFlag is true will return false
  ck_assert_int_eq(dmsCRegisterTriggers(1),1);
  ck_assert_int_eq(dmsCRegisterTriggers(0),1);
}
END_TEST

START_TEST(test_dmsCPIInitDMS)
{
  putenv("DEBUGDMS=1");
  putenv("DEBUG_IDB=1");
  putenv("DMS_FIND_UNMANAGED=1"); 
  putenv("DMS_DO_NOT_FIND_UNMANAGED=1"); 
  putenv("TURBO_DMSERVER=TWIN");
  putenv("TURBO_DEBUG=1");
  dmsCPIInitDMS("idmb","idmb",TRUE);
  unsetenv("DEBUGDMS");
  unsetenv("DEBUG_IDB");
  unsetenv("DMS_FIND_UNMANAGED");
  unsetenv("DMS_DO_NOT_FIND_UNMANAGED");
  unsetenv("TURBO_DMSERVER");
  unsetenv("TURBO_DEBUG");
}
END_TEST

/**************************************************************
TEST CASE  : test_dmsCCheckEnvSettings
DESCRIPTION: Test dmsCCheckEnvSettings() able to return TRUE
***************************************************************/
START_TEST(test_dmsCCheckEnvSettings)
{
  char *a[] = {"envTest"};
  char **s=a;

   ck_assert_int_eq(dmsCCheckEnvSettings(s),1);
}
END_TEST

START_TEST(test_dmsCExitDMS)
{
  dmsCExitDMS();
}
END_TEST

START_TEST(test_dmsCParseDmsPathFile)
{
  ck_assert_int_eq(dmsCParseDmsPathFile("/tmp/fakepath"),0);
}
END_TEST

START_TEST(test_dmsCReadonlyCellView)
{
  char *path;
  ck_assert_int_eq(dmsCReadonlyCellView("fake","fake",&path),29);
}
END_TEST

START_TEST(test_dmsCReadonlyCellViewType)
{
  char *path, *vtName, *vName;
  ck_assert_int_eq(dmsCReadonlyCellViewType("fake","fake",&vtName, &vName, &path) ,29);
}
END_TEST

START_TEST(test_dmsCCheckinCellView)
{
  char *path;
  ck_assert_int_eq(dmsCCheckinCellView("fake","fake","fake", 0,"log", &path) ,34);
}
END_TEST

START_TEST(test_dmsCUpdateCellView)
{
  char *path;
  ck_assert_int_eq(dmsCUpdateCellView("fake","fake","fake", 0,"log", &path) ,34);
}
END_TEST

START_TEST(test_dmsCCancelCheckoutCellView)
{
  char *path;
  ck_assert_int_eq(dmsCCancelCheckoutCellView("fake","fake", &path) ,46);
}
END_TEST

START_TEST(test_dmsCEditCellView)
{
  char *path;
  ck_assert_int_eq(dmsCEditCellView("fake","fake", &path) ,46);
}
END_TEST

START_TEST(test_dmsCCheckoutCellView)
{
  char *path;
  ck_assert_int_eq(dmsCCheckoutCellView("fake","fake", &path) ,46);
}
END_TEST

START_TEST(test_dmsCInfoCellView)
{
  char *path;
  ck_assert_int_eq(dmsCInfoCellView(1,"fake","fake", &path) ,1);
}
END_TEST

START_TEST(test_dmsCReadOrEditByPolicy)
{
  char *path;
  ck_assert_int_eq(dmsCReadOrEditByPolicy("fake","fake", &path) ,29);
}
END_TEST

START_TEST(test_dmsCGetLibPath)
{
  char *path;
  ck_assert_int_eq(dmsCGetLibPath("fake",&path) ,105);
}
END_TEST

START_TEST(test_dmsCGetRealLibPath)
{
  char *path;
  ck_assert_int_eq(dmsCGetRealLibPath("fake",&path) ,105);
}
END_TEST

START_TEST(test_dmCGenObjRel)
{
  ck_assert_int_eq(dmCGenObjRel("fake","fake",0) ,0);
  ck_assert_int_eq(dmCGenObjRel("fake","fake",1) ,0);
  ck_assert_int_eq(dmCGenObjRel("fake","fake",2) ,0);
  ck_assert_int_eq(dmCGenObjRel("fake","fake",3) ,0);
  ck_assert_int_eq(dmCGenObjRel("fake","fake",4) ,0);
  ck_assert_int_eq(dmCGenObjRel("fake","fake",5) ,0);
}
END_TEST

START_TEST(test_dmsCSaveCellView)
{
  char *path;
  ck_assert_int_eq(dmsCSaveCellView("fake","fake",&path) ,94);
}
END_TEST

START_TEST(test_dmsCGetAllVersions)
{
  ck_assert_int_eq(dmsCGetAllVersions("utlib","test","lnf","1") ,0);
}
END_TEST

START_TEST(test_dmsCGetMultipleViews)
{
  char *path;
  ck_assert_int_eq(dmsCGetMultipleViews(&path) ,0);
}
END_TEST

START_TEST(test_dmsCGetMultipleExts)
{
  char *path;
  ck_assert_int_eq(dmsCGetMultipleExts(&path) ,0);
}
END_TEST

START_TEST(test_dmsCCreateCellView)
{
  char *path;
  ck_assert_int_eq(dmsCCreateCellView("test2","lnf",&path) ,81);
}
END_TEST

START_TEST(test_dmsCCreateUnixFile)
{
  char *path;
  ck_assert_int_eq(dmsCCreateUnixFile("test2","lnf",&path) ,0);
}
END_TEST

Suite * C_API_1_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("dmsC_API_1");
  
    tc_core = tcase_create("Core");
   
    tcase_add_checked_fixture(tc_core, api_1_setup, api_1_teardown);

    tcase_add_test(tc_core, test_coralCDisableObsoleteMessages);
    tcase_add_test(tc_core, test_dmsCRegisterUserTriggers_1);
    tcase_add_test(tc_core, test_dmsCRegisterTriggers);
    tcase_add_test(tc_core, test_dmsCPIInitDMS);
    tcase_add_test(tc_core, test_dmsCExitDMS);
    tcase_add_test(tc_core, test_dmsCReadonlyCellView);
    tcase_add_test(tc_core, test_dmsCReadonlyCellViewType);
    tcase_add_test(tc_core, test_dmsCCheckinCellView);
    tcase_add_test(tc_core, test_dmsCUpdateCellView);
    tcase_add_test(tc_core, test_dmsCCancelCheckoutCellView);
    tcase_add_test(tc_core, test_dmsCCheckoutCellView);
    tcase_add_test(tc_core, test_dmsCInfoCellView);
    tcase_add_test(tc_core, test_dmsCEditCellView);
    tcase_add_test(tc_core, test_dmsCReadOrEditByPolicy);
    tcase_add_test(tc_core, test_dmsCGetLibPath);
    tcase_add_test(tc_core, test_dmsCGetRealLibPath);
    tcase_add_test(tc_core, test_dmsCCheckEnvSettings);
    tcase_add_test(tc_core, test_dmsCParseDmsPathFile);
    tcase_add_test(tc_core, test_dmCGenObjRel);
    tcase_add_test(tc_core, test_dmsCSaveCellView);
    tcase_add_test(tc_core, test_dmsCGetAllVersions);
    tcase_add_test(tc_core, test_dmsCGetMultipleViews);
    tcase_add_test(tc_core, test_dmsCGetMultipleExts);
    tcase_add_test(tc_core, test_dmsCCreateCellView);
    tcase_add_test(tc_core, test_dmsCCreateUnixFile);
    suite_add_tcase(s, tc_core);
    
    return s;
}
