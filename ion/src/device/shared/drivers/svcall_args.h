#ifndef ION_DEVICE_SHARED_SVCALL_ARGS_H
#define ION_DEVICE_SHARED_SVCALL_ARGS_H

void svcall(unsigned int svcNumber, int argc = 0, const char * argv[] = nullptr);

void setSvcallArgs(int argc, const char * argv[]);

void getSvcallArgs(int argc, const char * argv[]);

#endif