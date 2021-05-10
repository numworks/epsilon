#ifndef ION_DEVICE_KERNEL_BOOT_ISR_H
#define ION_DEVICE_KERNEL_BOOT_ISR_H

#include <boot/isr.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void isr_systick();
void keyboard_handler();
void tim2_handler(); // Event spinner/breaker
void tim4_handler(); // Keyboard debouncer
void tim12_handler(); // Event spinner hiding
void svcall_handler_as();
void svcall_handler(uint32_t processStackPointer, uint32_t exceptReturn, uint32_t svcNumber);
void pendsv_handler();

#ifdef __cplusplus
}
#endif

#endif
