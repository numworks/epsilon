#ifndef ION_DEVICE_BOOT_ISR_H
#define ION_DEVICE_BOOT_ISR_H

#ifdef __cplusplus
extern "C" {
#endif

void bf_abort();
void uf_abort();
void nmi_abort();
// Here and below, we are doing operations on the abort handler, not the opposite
void abort_init();
void abort_core(const char *);
void abort_screen(const char *);
void abort_sleeping();
void abort_economy();
void start();
void abort();
void isr_systick();

#ifdef __cplusplus
}
#endif

#endif
