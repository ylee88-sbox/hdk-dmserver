#include "cmars.h"
#include <Mars/Probe/Probe.h>
using namespace mars;

int cmarsABI(){
  return CMARS_ABI;
}


/*
Mars::DefaultStem("test");
Mars::DefaultTool("tool name");
Mars::DefaultBlock("block name");
Mars::DefaultToolVersion("1.1.1");
Mars::DefaultFlow("mainFlow|subFlow|ID");
*/

void MarsDefaults(
  const char *stem,
  const char *blockname,
  const char *toolname,
  const char *toolversion,
  const char *flow
){

Mars::DefaultStem(stem);
Mars::DefaultTool(toolname);
Mars::DefaultBlock(blockname);
Mars::DefaultToolVersion(toolversion);
Mars::DefaultFlow(flow);
}

/* Mars::Session().Block( 'new block name' ); */
void MarsSessionBlock(
  const char *block
){
  Mars::Session().Block(block);
}

/*  Mars::Session().Flow( 'mainflow|'+'newsubflow'+'|ID' ); */
void MarsSessionFlow(
  const char *flow
){
  Mars::Session().Flow(flow);
}

/* cout << Mars::Session().Block() << endl; */
const char *MarsSessionBlockGet(){
  return Mars::Session().Block().c_str();
}

/* Probe mars_temp ("routine name"); */
void *MarsProbe(
  const char *routineName
){
  return new Probe (routineName);
}


/* mars_temp.Quit(); */
void MarsProbeQuit(
  void *ThisMarsProbe
){
  ((Probe *)(ThisMarsProbe))->Quit();
  delete ThisMarsProbe;
}

/* mars_temp.Set("key", "value"); */
void MarsProbeSet(
  void *ThisMarsProbe,
  const char *probekey,
  const char *probeValue
){
  ((Probe *)(ThisMarsProbe))->Set(
    probekey,
    probeValue
  );
}

/* mars_temp.Send */
void MarsProbeSend(
  void *ThisMarsProbe,
  const char *configuration
){
  ((Probe *)(ThisMarsProbe))->Send(configuration);
}

/*  mars_temp.Block("new block name"); */
void MarsProbeBlock(
  void *ThisMarsProbe,
  const char *blockName
){
  ((Probe *)(ThisMarsProbe))->Block(blockName);
}

/* mars_temp.User("new user name"); */
void MarsProbeUser(
  void *ThisMarsProbe,
  const char *userName
){
  ((Probe *)(ThisMarsProbe))->User(userName);
}

/* cout << mars_temp.Block() << endl; */
const char *MarsProbeBlockGet(
  void *ThisMarsProbe
){
  return ((Probe *)(ThisMarsProbe))->Block().c_str();
}

/* cout << mars_temp.Routine() << endl; */
const char *MarsProbeRoutineGet(
  void *ThisMarsProbe
){
  return ((Probe *)(ThisMarsProbe))->Routine().c_str();
}
