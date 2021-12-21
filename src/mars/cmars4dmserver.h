#ifndef MARS4DMSERVER
#define MARS4DMSERVER

#include "cmars.h"

void init_mars(void) __attribute__((constructor));
void shutdowm_mars(void) __attribute__((destructor));

void* mars_function_enter(const char*routine);
void mars_function_exit(void *);

#endif
