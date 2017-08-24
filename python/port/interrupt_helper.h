#ifndef PYTHON_INTERRUPT_HELPER_H
#define PYTHON_INTERRUPT_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* shouldInterrupt effectively does something once every 20000 calls. It checks
 * if a key is down to raise an interruption flag. */

void shouldInterrupt();

#ifdef __cplusplus
}
#endif

#endif
