#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>

void dmsC_dm_setup(void)
{
   putenv("DEBUGDMS=1");
}
 
 void dmsC_dm_teardown(void)
{

}

START_TEST(test_dmsGetCdslibFileName)
{
  ck_assert_int_eq(dmsGetCdslibFileName("fake"),1);
}
END_TEST


Suite * dmsC_dm_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("dmsC_dm_suite");
  
    tc_core = tcase_create("Core");
   
    tcase_add_checked_fixture(tc_core, dmsC_dm_setup, dmsC_dm_teardown);

    tcase_add_test(tc_core, test_dmsGetCdslibFileName);
    suite_add_tcase(s, tc_core);
    
    return s;
}
