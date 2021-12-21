
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>

void dmsC_functions2_setup(void)
{
  putenv("DEBUGDMS=1");
}
 
void dmsC_functions2_teardown(void)
{
}

START_TEST(test_dmsPutBackSlashBeforeQuote_str_empty)
{

  dmsPutBackSlashBeforeQuote("\"d\"",1);
  
}
END_TEST

START_TEST(test_matchToolvName)
{
   const char *tvNameList;
   const char *tName;
   const char *vName;

   tvNameList = "tvNameList";
   tName = "tName";
   vName = "vName";
   ck_assert_int_eq(matchToolvName(tvNameList,tName,vName),0);
}
END_TEST


Suite * dmsC_functions2_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("dmsC_functions2_suite");
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, dmsC_functions2_setup,dmsC_functions2_teardown );

    tcase_add_test(tc_core, test_dmsPutBackSlashBeforeQuote_str_empty);
    tcase_add_test(tc_core, test_matchToolvName);
    suite_add_tcase(s, tc_core);
    return s;
}

