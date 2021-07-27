#ifndef ION_DEVICE_KERNEL_BOOT_ISR_H
#define ION_DEVICE_KERNEL_BOOT_ISR_H

#include <shared/boot/isr.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void isr_systick();
void keyboard_handler();
void tim2_handler(); // Event spinner/breaker
void tim4_handler(); // Keyboard debouncer
void tim5_handler(); // Keyboard poll while a key is pressed
void tim6_handler(); // Ion::CircuitBreaker bypasses the lock after a while
void svcall_handler_as();
void svcall_handler(uint32_t processStackPointer, uint32_t exceptReturn, uint32_t svcNumber);
void pendsv_handler();

#ifdef __cplusplus
}
#endif

#endif
