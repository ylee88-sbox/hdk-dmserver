#include <turbo/dmsT_API.h>
#include <turbo/dmsT_manager.h>

#include <string.h>

extern "C" int dmsTInit()
{
   return (dmsT_manager::Instance())?1:0;
}

extern "C" void dmsTSetDebug()
{
   dmsT_manager::Instance()->set_debug();
}

extern "C" void dmsTExit()
{
   dmsT_manager::Instance()->exit();
}

extern "C" int dmsTInsertCellRecord(const char *l, const char *t, const char *n, const char *v, const char *d, int h, int td)
{
   //std:: cout << l << "," << t << "," << n << "," << v << "," << d << "," << h << "," << td << std::endl;
   return (int)dmsT_manager::Instance()->insert_record(l, t, n, v, d, h, td);
}

extern "C" int dmsTInsertCellRecordAndWholeDir(const char *l, const char *t, const char *n, const char *v, const char *d, int h, int td)
{
   return  (int)dmsT_manager::Instance()->insert_record_and_whole_dir(l, t, n, v, d, h, td);
}

extern "C" const char *dmsTFindCellInCache(const char *n, const char *v) 
{ 
   return dmsT_manager::Instance()->find_cell_in_cache(n, v);
}


//
// get last stuff
//

static char _lCN[1024];
static char _lLN[1024];
static char _lVN[1024];

extern "C" const char *dmsTGetLastCellName()    // get last "marked" cell name
{
   cell_record *lmp = dmsT_manager::Instance()->get_marked_list_element();
   if (!lmp)
      return 0;
   strcpy(_lCN, lmp->cell_name.c_str());
   return _lCN;
}

extern "C" const char *dmsTGetLastLibraryName() // get last "marked" library name
{
   cell_record *lmp = dmsT_manager::Instance()->get_marked_list_element();
   if (!lmp)
      return 0;
   strcpy(_lLN, lmp->lib_name.c_str());
   return _lLN;
}

extern "C" const char *dmsTGetLastVersion()     // get last "marked" version
{
   cell_record *lmp = dmsT_manager::Instance()->get_marked_list_element();
   if (!lmp)
      return 0;
   strcpy(_lVN, lmp->version.c_str());
   return _lVN;
}

extern "C" int  dmsTSetTwinTurboMode() 
{
   return (int)dmsT_manager::Instance()->set_twin_turbo();
}

extern "C" int dmsTGetTwinTurboMode()
{
   return (int)dmsT_manager::Instance()->get_twin_turbo();
}

