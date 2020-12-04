#ifndef ION_DEVICE_SHARED_SVCALL_ARGS_H
#define ION_DEVICE_SHARED_SVCALL_ARGS_H

void setArg(int argc, const char * argv);

const char * getArg(int argc);

void svcArgs(int argc, const char * argv[]);

void svcGetArgs(int argc, const char * argv[]);

#endif