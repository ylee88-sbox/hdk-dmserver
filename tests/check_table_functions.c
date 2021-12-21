
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <dmsC.h>
#include <dmsC_defines.h>
#include <time.h> 

void table_functions_setup(void)
{
  putenv("DEBUGDMS=1");
}
 
void table_functions_teardown(void)
{
}

START_TEST(test_idmGetExtension)
{
   char * vName="a";
   int i;
   char* name_with_extension;
   name_with_extension = malloc(strlen(vName)+20200); 
   strcpy(name_with_extension, vName); /* copy name into the new var */
   for(i=0; i<4003; i++){
      strcat(name_with_extension, "vName"); /* add the extension */
   }

   ck_assert(idmGetExtension(name_with_extension,0) == NULL);
}
END_TEST

START_TEST(test_idmPrintGlobalTable)
{
   idmPrintGlobalTable();
}
END_TEST

Suite * table_functions_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("table_functions_suite");

    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, table_functions_setup, table_functions_teardown);

    tcase_add_test(tc_core, test_idmGetExtension);
    tcase_add_test(tc_core, test_idmPrintGlobalTable);

    suite_add_tcase(s, tc_core);

    return s;
}

