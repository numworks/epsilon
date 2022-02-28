#ifndef ION_DEVICE_SHARED_BOOT_ISR_H
#define ION_DEVICE_SHARED_BOOT_ISR_H

#define INITIALISATION_VECTOR_SIZE 0x71

#ifdef __cplusplus
extern "C" {
#endif

void start();
void abort();
void bus_fault_handler();

#ifdef __cplusplus
}
#endif

#endif
