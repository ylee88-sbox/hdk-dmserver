#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>

void idb_functions_setup(void)
{
  putenv("DEBUGDMS=1");
}
 
void idb_functions_teardown(void)
{
}


START_TEST(test_idb_capture_cdn_vendor_warning)
{
  ck_assert_str_eq(idb_capture_cdn_vendor_warning(),"");
}
END_TEST

START_TEST(test_idb_get_cdn_vendor_string)
{
  ck_assert_str_eq(idb_get_cdn_vendor_string(),"*IDMS - SYNCHRONICITY*");
  ck_assert_str_eq(idb_set_dm_vendor_string("set vendor String"),"set vendor String");
  ck_assert_str_eq(idb_get_cdn_vendor_string(),"set vendor String");
}
END_TEST

START_TEST(test_idb_clear_errors)
{
  ck_assert_int_eq(idb_clear_errors(),0);
}
END_TEST

START_TEST(test_idb_OR_error_code)
{
  ck_assert_int_eq(idb_OR_error_code(1),1);
}
END_TEST

START_TEST(test_idb_is_collection_obj)
{
  ck_assert_int_eq(idb_is_collection_obj("0","test","lnf"),0);
}
END_TEST

START_TEST(test_idb_get_locker)
{
  ck_assert_int_eq(idb_get_locker("utlib","test","lnf","/tmp/fake","0"),0);
  ck_assert_int_eq(idb_get_locker("utlib","test","lnf","/tmp/fake","1"),0);
}   
END_TEST

START_TEST(test_idb_check_success)
{
  ck_assert_int_eq(idb_check_success(),1);
}   
END_TEST

START_TEST(test_idb_cellview_id)
{
  ck_assert_int_eq(idb_set_cellview_id(NULL),0);
  ck_assert_int_eq(idb_set_cellview_id(1),1);

  ck_assert_int_eq(idb_get_cellview_id(),1);
}   
END_TEST

START_TEST(test_idb_check_cell_in_lib)
{
  ck_assert_int_eq(idb_check_cell_in_lib(NULL,"test"),0);
  ck_assert_int_eq(idb_check_cell_in_lib("utlib","test"),0);
}   
END_TEST

START_TEST(test_idb_last_directory_cell)
{
  ck_assert_str_eq(idb_set_last_directory_cell(NULL),"");
  ck_assert_str_eq(idb_get_last_directory1_cell(),"");
}
END_TEST

START_TEST(test_idb_last_cell_name)
{
  ck_assert_str_eq(idb_set_last_cell_name(),"");
  ck_assert_str_eq(idb_get_last_cell_name(),"");
  ck_assert_str_eq(idb_set_last_cell_name(NULL),"");
}
END_TEST

START_TEST(test_idb_last_workfile)
{
  ck_assert_str_eq(idb_set_last_workfile(NULL),"");
  ck_assert_str_eq(idb_set_last_workfile("fake"),"fake");
  ck_assert_str_eq(idb_get_last_workfile(),"fake");
}
END_TEST


Suite * idb_functions_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("idb_functions_suite");
    tc_core = tcase_create("Core");
   
    tcase_add_checked_fixture(tc_core, idb_functions_setup, idb_functions_teardown);
    tcase_add_test(tc_core, test_idb_capture_cdn_vendor_warning);
    tcase_add_test(tc_core, test_idb_get_cdn_vendor_string);
    tcase_add_test(tc_core, test_idb_clear_errors);
    tcase_add_test(tc_core, test_idb_OR_error_code);
    tcase_add_test(tc_core, test_idb_is_collection_obj);
    tcase_add_test(tc_core, test_idb_get_locker);
    tcase_add_test(tc_core, test_idb_check_success);
    tcase_add_test(tc_core, test_idb_cellview_id);
    tcase_add_test(tc_core, test_idb_check_cell_in_lib);
    tcase_add_test(tc_core, test_idb_last_cell_name);
    tcase_add_test(tc_core, test_idb_last_directory_cell);
    tcase_add_test(tc_core, test_idb_last_workfile);
    suite_add_tcase(s, tc_core);
    return s;
}
