#ifndef ION_DEVICE_SHARED_SVCALL_H
#define ION_DEVICE_SHARED_SVCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#define svc(code) asm volatile ("svc %[immediate]"::[immediate] "I" (code))

#define SVC_CLOCK_LOW_FREQUENCY 1
#define SVC_CLOCK_STANDARD_FREQUENCY 2
#define SVC_POWER_SLEEP_OR_STOP 3
#define SVC_POWER_STANDBY 4
#define SVC_DFU 5
#define SVC_RESET_CORE 6
#define SVC_EXAM_MODE_TOGGLE 7
#define SVC_PUSH_RECT 8
#define SVC_PUSH_RECT_UNIFORM 9

#ifdef __cplusplus
}
#endif

void svcall(unsigned int svcNumber, int argc = 0, void * argv[] = nullptr);

void getSvcallArgs(int argc, void * argv[]);

void setSvcallArgs(int argc, void * argv[]);

#endif
