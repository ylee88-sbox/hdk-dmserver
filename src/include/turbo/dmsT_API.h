#ifndef __DMST_API_H
#define __DMST_API_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Prototypes for exported "C" functions */

int dmsTInit();

void dmsTSetDebug();

void dmsTExit();

int dmsTInsertCellRecord(const char *l, const char *t, const char *n, const char *v, const char *d, int h, int td);
int dmsTInsertCellRecordAndWholeDir(const char *l, const char *t, const char *n, const char *v, const char *d, int h, int td);

const char *dmsTFindCellInCache(const char *n, const char *v);

const char *dmsTGetLastCellName();    // get last "marked" cell name
const char *dmsTGetLastLibraryName(); // get last "marked" library name
const char *dmsTGetLastVersion();     // get last "marked" version

int dmsTSetTwinTurboMode(); // set twin turbo mode
int dmsTGetTwinTurboMode(); // false if not set

#ifdef __cplusplus
  }
#endif

#endif
