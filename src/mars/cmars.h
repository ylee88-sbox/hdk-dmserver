#ifndef MARS_C_API
#define MARS_C_API

#ifdef __cplusplus
extern "C" {
#endif

#define CMARS_EXPORT __attribute__ ((visibility ("default")))
#define CMARS_LOCAL __attribute__ ((visibility ("hidden")))

#define CMARS_ABI 0
typedef int (*cmarsABIType)();
CMARS_EXPORT int cmarsABI();

/*
Mars::DefaultStem("test");
Mars::DefaultTool("tool name");
Mars::DefaultBlock("block name");
Mars::DefaultToolVersion("1.1.1");
Mars::DefaultFlow("mainFlow|subFlow|ID");
*/
typedef void (*MarsDefaultsType)(
  const char *stem,
  const char *blockname,
  const char *toolname,
  const char *toolversion,
  const char *flow
);
CMARS_EXPORT void MarsDefaults(
  const char *stem,
  const char *blockname,
  const char *toolname,
  const char *toolversion,
  const char *flow
);

/* Mars::Session().Block( 'new block name' ); */
typedef void (*MarsSessionBlockType)(
  const char *block
);
CMARS_EXPORT void MarsSessionBlock(
  const char *block
);

/*  Mars::Session().Flow( 'mainflow|'+'newsubflow'+'|ID' ); */
typedef void (*MarsSessionFlowType)(
  const char *flow
);
CMARS_EXPORT void MarsSessionFlow(
  const char *flow
);

/* cout << Mars::Session().Block() << endl; */
typedef const char *(*MarsSessionBlockGetType)();
const char * MarsSessionBlockGet() ;

/* Probe mars_temp ("routine name"); */
typedef void *(*MarsProbeType)(
  const char *routineName
);
CMARS_EXPORT void * MarsProbe(
  const char *routineName
);


/* mars_temp.Quit(); */
typedef void (*MarsProbeQuitType)(
  void *ThisMarsProbe
);
CMARS_EXPORT void MarsProbeQuit(
  void *ThisMarsProbe
);

/* mars_temp.Set("key", "value"); */
typedef void (*MarsProbeSetType)(
  void *ThisMarsProbe,
  const char *probekey,
  const char *probeValue
);
CMARS_EXPORT void MarsProbeSet(
  void *ThisMarsProbe,
  const char *probekey,
  const char *probeValue
);


/* mars_temp.Send */
typedef void (*MarsProbeSendType)(
  void *ThisMarsProbe,
  const char *configuration
);

CMARS_EXPORT void MarsProbeSend(
  void *ThisMarsProbe,
  const char *configuration
);

/*  mars_temp.Block("new block name"); */
typedef void (*MarsProbeBlockType)(
  void *ThisMarsProbe,
  const char *blockName
);

CMARS_EXPORT void MarsProbeBlock(
  void *ThisMarsProbe,
  const char *blockName
);

/* mars_temp.User("new user name"); */
typedef void (*MarsProbeUserType)(
  void *ThisMarsProbe,
  const char *userName
);
CMARS_EXPORT void MarsProbeUser(
  void *ThisMarsProbe,
  const char *userName
);

/* cout << mars_temp.Block() << endl; */
typedef const char *(*MarsProbeBlockGetType)(
  void *ThisMarsProbe
);
CMARS_EXPORT const char * MarsProbeBlockGet(
  void *ThisMarsProbe
);

/* cout << mars_temp.Routine() << endl; */
typedef const char *(*MarsProbeRoutineGetType)(
  void *ThisMarsProbe
);
CMARS_EXPORT const char * MarsProbeRoutineGet(
  void *ThisMarsProbe
);

#ifdef __cplusplus
}
#endif
#endif
