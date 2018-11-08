#ifndef PYTHON_PORT_HELPERS_H
#define PYTHON_PORT_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

/* should_interrupt effectively does something once every 20000 calls. It checks
 * if a key is down to raise an interruption flag. */
bool micropython_port_should_interrupt(bool);

#ifdef __cplusplus
}
#endif

#endif
