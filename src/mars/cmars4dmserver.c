#include "cmars4dmserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

typedef struct CMARS_fn {
  void *so_handle;
  cmarsABIType cmarsABI;
  MarsDefaultsType MarsDefaults;
  MarsSessionBlockType MarsSessionBlock;
  MarsSessionFlowType MarsSessionFlow;
  MarsSessionBlockGetType MarsSessionBlockGet;
  MarsProbeType MarsProbe;
  MarsProbeQuitType MarsProbeQuit;
  MarsProbeSetType MarsProbeSet;
  MarsProbeSendType MarsProbeSend;
  MarsProbeBlockType MarsProbeBlock;
} CMARS_fn;

static CMARS_fn cmars_table;

void init_mars(void){
  char *path = NULL;
  if(!getenv("DMSERVER_LIB_DIR")) return;
  asprintf(&path, "%s/%s", getenv("DMSERVER_LIB_DIR"), "libcmars.so");
  if(!path)
    goto endpart;

  if(!(cmars_table.so_handle = dlopen(path, RTLD_NOW))) goto endpart;
  if(!(cmars_table.cmarsABI = (cmarsABIType) dlsym(cmars_table.so_handle, "cmarsABI")) || cmars_table.cmarsABI() != CMARS_ABI) goto clear_struct;

  if(!(cmars_table.MarsDefaults = (MarsDefaultsType)dlsym(cmars_table.so_handle, "MarsDefaults"))) goto clear_struct;
  if(!(cmars_table.MarsSessionBlock = (MarsSessionBlockType)dlsym(cmars_table.so_handle, "MarsSessionBlock"))) goto clear_struct;
  if(!(cmars_table.MarsSessionFlow = (MarsSessionFlowType)dlsym(cmars_table.so_handle, "MarsSessionFlow"))) goto clear_struct;
  if(!(cmars_table.MarsSessionBlockGet = (MarsSessionBlockGetType)dlsym(cmars_table.so_handle, "MarsSessionBlockGet"))) goto clear_struct;
  if(!(cmars_table.MarsProbe = (MarsProbeType)dlsym(cmars_table.so_handle, "MarsProbe"))) goto clear_struct;
  if(!(cmars_table.MarsProbeQuit = (MarsProbeQuitType)dlsym(cmars_table.so_handle, "MarsProbeQuit"))) goto clear_struct;
  if(!(cmars_table.MarsProbeSet = (MarsProbeSetType)dlsym(cmars_table.so_handle, "MarsProbeSet"))) goto clear_struct;
  if(!(cmars_table.MarsProbeSend = (MarsProbeSendType)dlsym(cmars_table.so_handle, "MarsProbeSend"))) goto clear_struct;
  if(!(cmars_table.MarsProbeBlock = (MarsProbeBlockType)dlsym(cmars_table.so_handle, "MarsProbeBlock"))) goto clear_struct;

  cmars_table.MarsDefaults("stem","blockname","dmserver",DMSVERSION,"flow");

  goto endpart;
  clear_struct:
  shutdowm_mars();

  endpart:
  free(path);
  return;
}

void shutdowm_mars(void) {
  if(cmars_table.so_handle) dlclose(cmars_table.so_handle);
  memset(&cmars_table, 0, sizeof(cmars_table));
}

void* mars_function_enter(const char*routine){
  return cmars_table.MarsProbe ? cmars_table.MarsProbe(routine) : NULL;
}
void mars_function_exit(void *This){
  return (This && cmars_table.MarsProbeQuit) ? cmars_table.MarsProbeQuit(This) : NULL;
}
