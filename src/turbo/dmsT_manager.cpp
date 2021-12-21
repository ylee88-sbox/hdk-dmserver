#include <turbo/dmsT_manager.h>

#include <string.h>
#include <algorithm>

dmsT_manager *dmsT_manager::_instance =  0;

dmsT_manager::~dmsT_manager() {
}

void dmsT_manager::exit() 
{
   if (!_debug)
      return;
  
  // dump_status();

}

void dmsT_manager::dump_status(std::ostream& os)
{
   os << "*dmsT* -I- Contents of records list" << std::endl;

   std::list<cell_record *>::iterator it;

   if (!_rec_list){
      os << "EMPTY" << std::endl;
      return;
   }
   for (it = _rec_list->begin(); it != _rec_list->end(); ++it){
      os << *(*it) << std::endl;
   }
}


dmsT_manager::error_code dmsT_manager::insert_record(const char *l, const char *t, const char *n, const char *v, const char *d, int h, int td){
   _last_list_element = new cell_record(l, t, n, v, d, h, td);
   if (_rec_list == 0){
      _rec_list = new std::list<cell_record *> ();
   }
   _rec_list->push_back(_last_list_element);

   // also add the record into the _hash_cells *** for the twin turbo mode only ***

   if (_twin_turbo) {
      if (_hash_cells == 0){
          _hash_cells = new Hash_Cell_Map ();
      }

      (*_hash_cells)[_last_list_element->cell_name.c_str()] = _last_list_element;
   }

   return SUCCESS;
}

const char * dmsT_manager::find_cell_in_cache(const char *cell_name)
{
   if (_hash_cells == 0){
      return 0;
   }

   Hash_Cell_Map_Const_It it = _hash_cells->find(cell_name);
   if (it == _hash_cells->end()){
      return 0;
   }

   const cell_record *r = it->second;
   _last_marked_list_element = (cell_record *)r; 
   return r->directory.c_str();
}


static char _sbuff[1024];
const char * dmsT_manager::find_cell_in_cache(const char *c, const char *v){
   strcpy(_sbuff, c);
   strcat(_sbuff, ".");
   strcat(_sbuff, v);
   return find_cell_in_cache(_sbuff);
}

//--------------------------------------------------------------------------------
// cashes all cells in cell directory. Also gets the version for the current cell 
//--------------------------------------------------------------------------------

//
// currently this is stuff "borrowed" out of /vobs/env/green/src -- will fix by calling back from there
//

static int no_fields = 5; // interested only up to versions

struct beg_end {
   const char *_b;
   const char *_e;
};

beg_end BE[10];

const int idx_name = 0;
const int idx_state= 1;
const int idx_ver  = 4;

const int Name_len = strlen("<Name>");
const int Cadence_len = strlen("Cadence");
const int File_len = strlen("File");

const char *coll_suffix = ".sync.cds";
const int coll_suffix_len = strlen(coll_suffix);

static char _dir_buff[1024];
static char _cont_buff[1024];
static char file_buff[1024];

static char _crt_name[1024];
static char _crt_ver[1024];
static char _crt_fpn[1024];

dmsT_manager::error_code dmsT_manager::insert_record_and_whole_dir(const char *l, const char *t, const char *n, const char *v, const char *d, int h, int td){
   // get the cell name to isolate dir name

   const char *cstart = d + strlen(d) -1;
   while ((cstart >= d) && (*cstart != '/'))
      cstart--;
  
   if (cstart <d) // incomplete info in fullpath
      return ERR_BAD_DATA;

   const char *crt_cell_name = cstart+1;

   // /.../..../<cell_name>
   // ^        ^
   // d        cstart
   //
   // getting the directory - from d to cstart 

   strncpy(_dir_buff, d, std::max((long)(cstart-d), 1024L));
   _dir_buff[cstart-d] = '\0';
   //std::cout << "current cell directory: " << _dir_buff << " cell: " << crt_cell_name << std::endl;

   // if no .SYNC/Contents file in the directory just bail out - might be a collection - not supported yet

   strcpy(_cont_buff,  _dir_buff); 
   strcat(_cont_buff, "/.SYNC/Contents");
   FILE *fcont = fopen(_cont_buff, "r");

   if (!fcont){ // just bail for now
      return ERR_NOT_SUPPORTED;
   }

   //   <Name><State><Vault><Branch><Version><DateFetched><Log><Conflict><StickyTag><MergedFrom>
   //    1     2      3      4       5        6            7    8         9          10
   //ndx 0     1      2      3       4


   bool header_found = false;
   while (fgets(file_buff, 1023, fcont)){
      if (! header_found){
         if (strstr(file_buff, "<Name>") != 0){
            header_found = true;
         }
         continue;
      }

      int crt_fields = 0;
      bool skip_type_G = false;
      const char *crt_pointer = file_buff;

      while  (crt_fields < no_fields){
         if ((BE[crt_fields]._b = strchr(crt_pointer, '<')) == 0)
            break;
         crt_pointer = BE[crt_fields]._b+1;

         if ((crt_fields == idx_state) && (*crt_pointer == 'G')) {
            skip_type_G = true;
            break;
         }

         if ((BE[crt_fields]._e = strchr(crt_pointer, '>')) == 0)
            break;
         crt_pointer =  BE[crt_fields]._e+1;

         crt_fields++;
      }
      if (crt_fields < no_fields) // incomplete line
         continue;

      //
      //Name
      //
      int oblen = BE[idx_name]._e-BE[idx_name]._b-1;
      strncpy(_crt_name,  BE[idx_name]._b+1, oblen);
      _crt_name[oblen] = '\0';

      strcpy(_crt_fpn, _dir_buff);
      strcat(_crt_fpn, "/");
      strcat(_crt_fpn, _crt_name);

      //const char *collp = strstr(out_buff, coll_suffix);

      // Ver
      oblen = BE[idx_ver]._e-BE[idx_ver]._b-1;
      strncpy(_crt_ver, BE[idx_ver]._b+1, oblen);
      _crt_ver[oblen] = '\0';

      // Collection or file
#if 0
      if (collp){
         oblen = Cadence_len;
      }
      else {
         oblen = File_len;
      }
#endif

      //std::cout << "FPN=" << _crt_fpn << " VER=" << _crt_ver << std::endl;

      int how_set = 1;
      bool main_cell = false;
      if (strcmp(_crt_name, crt_cell_name) == 0){
         how_set = 0;
         main_cell = true;
      }

      if ((_twin_turbo) || (main_cell)){ //  function actually enabled only for twin turbo, otherwise just return attributes for current cell by
                                         //  setting _last_marked_list_element;
         insert_record(l, t, _crt_name, _crt_ver, _crt_fpn, how_set, td);
      }

      if (main_cell){ // at this point we are done if not twin turbo - we just found our cell in the file so we do not need to read the trest since we are not cashing the entire dir contents
         _last_marked_list_element = _last_list_element;
         if (!_twin_turbo){
            fclose(fcont);
            return SUCCESS; 
         }
      }
   }

   fclose(fcont);

   return SUCCESS;
}
