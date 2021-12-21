#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>
#include <dmsC_new_vars.h>
//#include <time.h>

void dmsC_API_new_setup(void)
{
  putenv("DEBUGDMS=1");
}

void dmsC_API_new_teardown(void)
{
}

START_TEST(test_setWarn)
{
   dmsCSetWarnPrompt("This is test warn");
   char * warning = dmsCGetWarnPrompt();
   ck_assert_str_eq(warning,"This is test warn");
}
END_TEST

START_TEST(test_setWarn_empty)
{
   char * warning = dmsCGetWarnPrompt();
   ck_assert_str_eq(warning,"*IDMS WARNING* ");
}
END_TEST

START_TEST(test_dmsCGetLastErrorCode)
{
   dmsLastErrorNumber = 77;
   ck_assert_int_eq(dmsCGetLastErrorCode(),77);
}
END_TEST

START_TEST(test_dmsCGetLastStatusCode)
{
   ck_assert_int_eq(dmsCGetLastStatusCode(),0);
}
END_TEST

START_TEST(test_dmsCPrintWarning)
{
   dmsCPrintWarning("test warn");
}
END_TEST

START_TEST(test_dmsCPrintError)
{
   dmsCPrintError("test error");
}
END_TEST

START_TEST(test_dmsCSetLastMessageSize)
{
   dmsCSetLastMessageSize(1);
}
END_TEST

START_TEST(test_dmsCSetGeneratedAttribute)
{
   ck_assert_int_eq(dmsCSetGeneratedAttribute("fake","fake","fake","fake","fake"),0);
}
END_TEST

START_TEST(test_dmsCGetLastVendorMessage)
{
   ck_assert_str_eq(dmsCGetLastVendorMessage(),"");
}
END_TEST

START_TEST(test_dmsCPathInitDMS)
{
  // ck_assert_int_eq(dmsCPathInitDMS("idmb","boot","dmsPath",0),14);
}
END_TEST

Suite * dmsC_API_new_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("dmsC_API_new_suite");
    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, dmsC_API_new_setup, dmsC_API_new_teardown);

    tcase_add_test(tc_core, test_setWarn_empty);
    tcase_add_test(tc_core, test_setWarn);
    tcase_add_test(tc_core, test_dmsCGetLastErrorCode);
    tcase_add_test(tc_core, test_dmsCGetLastStatusCode);
    tcase_add_test(tc_core, test_dmsCPrintWarning);
    tcase_add_test(tc_core, test_dmsCPrintError);
    tcase_add_test(tc_core, test_dmsCSetLastMessageSize);    
    tcase_add_test(tc_core, test_dmsCSetGeneratedAttribute);
    tcase_add_test(tc_core, test_dmsCGetLastVendorMessage);
    tcase_add_test(tc_core, test_dmsCPathInitDMS);
 
    suite_add_tcase(s, tc_core);
    return s;
}

