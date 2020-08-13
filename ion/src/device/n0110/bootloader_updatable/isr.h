#ifndef ION_DEVICE_BOOT_ISR_H
#define ION_DEVICE_BOOT_ISR_H

#ifdef __cplusplus
extern "C" {
#endif

void start();
void abort();
void isr_systick();
//void svcall_handler(unsigned int * args);

#ifdef __cplusplus
}
#endif

#endif
