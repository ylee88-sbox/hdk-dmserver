#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>

void dmsC_status_setup(void)
{
   dmsCPIInitDMS("idmb","idmb",TRUE);
}
 
void dmsC_status_teardown(void)
{
}

START_TEST(test_dmsGetLastToolConfName)
{
  ck_assert_str_eq(dmsGetLastToolConfName(),"");
}
END_TEST

START_TEST(test_dmsGetLastPathConfName)
{
  ck_assert_str_eq(dmsGetLastPathConfName(),"");
}
END_TEST

START_TEST(test_dmsGetLastFullPathName)
{
  ck_assert_str_eq(dmsGetLastFullPathName(),"");
}
END_TEST

START_TEST(test_dmsGetLastLibPath)
{
  ck_assert_str_eq(dmsGetLastLibPath(),"");
}
END_TEST

START_TEST(test_dmsCGetAttribute)
{
  char *attType;
  char *attValue;
  ck_assert_int_eq(dmsCGetAttribute("utlib","test","lnf","1.1","",&attType, &attValue),106);
}
END_TEST


START_TEST(test_deallocate_dms_structs)
{
  ck_assert_int_eq(deallocate_dms_structs(),1);
}
END_TEST

Suite * dmsC_status_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("dmsC_status_suite");
    tc_core = tcase_create("Core");
   
    tcase_add_checked_fixture(tc_core, dmsC_status_setup, dmsC_status_teardown);

    tcase_add_test(tc_core, test_dmsGetLastToolConfName);
    tcase_add_test(tc_core, test_dmsGetLastPathConfName);
    tcase_add_test(tc_core, test_dmsGetLastFullPathName);
    tcase_add_test(tc_core, test_dmsGetLastLibPath);
    tcase_add_test(tc_core, test_dmsCGetAttribute);
    tcase_add_test(tc_core, test_deallocate_dms_structs);
    suite_add_tcase(s, tc_core);
    return s;
}
