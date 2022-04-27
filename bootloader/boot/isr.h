#ifndef ION_DEVICE_BOOT_ISR_H
#define ION_DEVICE_BOOT_ISR_H

#ifdef __cplusplus
extern "C" {
#endif

void start();
void abort();
void isr_systick();

// Fault handlers

void hard_fault_handler();
void mem_fault_handler();
void usage_fault_handler();
void bus_fault_handler();

#ifdef __cplusplus
}
#endif

#endif
