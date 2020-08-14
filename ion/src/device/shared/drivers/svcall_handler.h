#ifndef ION_DEVICE_BOOT_SVCALL_HANDLER_H
#define ION_DEVICE_BOOT_SVCALL_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#define svc(code) asm volatile ("svc %[immediate]"::[immediate] "I" (code))

#define SVC_SYSTICK_LOW_FREQUENCY 1
#define SVC_SYSTICK_HIGH_FREQUENCY 2
#define SVC_POWER_SLEEP_OR_STOP 3
#define SVC_POWER_STANDBY 4
#define SVC_RESET_CORE 5

#ifdef __cplusplus
}
#endif

#endif
