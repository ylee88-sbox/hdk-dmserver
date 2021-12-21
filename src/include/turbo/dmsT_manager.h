#ifndef __DMST_MANAGER
#define __DMST_MANAGER

#include <iostream>
#include <fstream>
#include <list>
#include <cstring>
#include <hash_map>

//------------------------------------------------------------------------------
// Helper stuff - might move
//------------------------------------------------------------------------------

class cell_record {
public:
   cell_record(const char *l = "", const char *t = "", const char *n= "", const char *v ="", const char *d = "", int h=0, int td=0){
      lib_name = l; type =t; cell_name =n; version = v; directory = d; how_set = h; type_of_dir = td;
   }

   std::string lib_name;
   std::string type;
   std::string cell_name; // includes the type like cell1.lnf
   std::string version;
   std::string directory;
   int how_set;     // 0 = old fashion way, 1 = cashed
   int type_of_dir; // 0 = sync library -- may be this should match the idms policy number 

   std::ostream& print (std::ostream& os) {
      os << lib_name << "," << type << "," << cell_name << "," << version << "," << directory << "," << how_set << "," << type_of_dir
         << std::endl;
      return os;
   }
   friend std::ostream& operator << (std::ostream& os, cell_record& cr){
      return cr.print(os);
   }
};

//--------------------------------------------------------
// To store the hash information for fast cell retrieval
//--------------------------------------------------------

struct eqstr {
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};


//
// Will be able to use const char * here if we are always keeping this in sync with the
// _rec_list which stores the cells in a list, including the name as a string
//
// Note again that the cell name includes the extension so we can retrieve it at once in hash
// Otherwise we would have conflicts like cell1.sch and cell1.lnf will come on the same hash entry then have a list etc...
// 

typedef  __gnu_cxx::hash_map<const char*, cell_record *,  __gnu_cxx::hash<const char*>, eqstr> Hash_Cell_Map;
typedef  __gnu_cxx::hash_map<const char*, cell_record *,  __gnu_cxx::hash<const char*>, eqstr>::const_iterator Hash_Cell_Map_Const_It;

//
// hash containing the list of managed/unmanaged directories
//

typedef  __gnu_cxx::hash_map<const char*, int,  __gnu_cxx::hash<const char*>, eqstr> Hash_Dir_Map;
typedef  __gnu_cxx::hash_map<const char*, int,  __gnu_cxx::hash<const char*>, eqstr>::const_iterator Hash_Dir_Map_Const_It;

//------------------------------------------------------------------------------
//
// This is the class managing the dmsT module. Implemented in singleton fashion
// to make sure it has only one instatiation. The user will still have to
// instantiate (once) dmsT_manager::_instance but that's it
//
//-----------------------------------------------------------------------------

//===============
// dmsT_manager
//===============

class dmsT_manager {
public:
   static dmsT_manager *Instance(){
      if (_instance == 0){
         _instance = new dmsT_manager();
      }
      return _instance;
   }

   enum error_code {SUCCESS=0, ERR_NOT_INITIALIZED, ERR_DIR_NOT_FOUND, ERR_NO_CONTENTS, ERR_CANNOT_CREATE_FILE, ERR_BAD_DATA, ERR_NOT_SUPPORTED};

   void dump_status(std::ostream& os = std::cout);

   //...........................................
   // Functions to be used through Instance()
   //...........................................

public:

//........................................................
// general functions with extern "C" dmsT correspondents
//........................................................

   void init() {Instance();};
   void set_debug() { Instance()->_debug = true;}
   void exit();

// handling the cash

   error_code insert_record(const char *l = "", const char *t = "", const char *n= "", const char *v ="", const char *d = "", int h=0, int td=0);
   error_code insert_record_and_whole_dir (const char *l = "", const char *t = "", const char *n= "", const char *v ="", const char *d = "", int h=0, int td=0); 

   const char* find_cell_in_cache (const char *cell_name);
   const char* find_cell_in_cache(const char *c, const char *v);

//.........................
// other public functions
//.........................

    cell_record * mark_last_list_element () {return _last_marked_list_element = _last_list_element;}
    cell_record * get_last_list_element () const {return _last_list_element;}
    cell_record * get_marked_list_element () const {return _last_marked_list_element;} 
    bool set_twin_turbo () {return _twin_turbo = true;}
    bool get_twin_turbo () const {return _twin_turbo;}

protected:
   dmsT_manager() :_rec_list(0), _hash_cells(0), _hash_dirs(0), _debug(false), _last_list_element(0), _last_marked_list_element(0), _twin_turbo(false){
      std::cout << "*dmsT* -I- initializing dmsT" << std::endl;
   }
   virtual ~dmsT_manager();
private:
   static dmsT_manager *_instance;
//------------------------------------------------------------------
// rest of the data structure storing the current cashed libraries
//------------------------------------------------------------------

   std::list<cell_record *> *_rec_list; 
   Hash_Cell_Map *_hash_cells;
   Hash_Dir_Map  *_hash_dirs;

   cell_record * _last_list_element;
   cell_record * _last_marked_list_element;

// flags

   bool _debug;      
   bool _twin_turbo;
};

#endif // __DMST_MANAGER
