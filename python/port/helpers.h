#ifndef PYTHON_PORT_HELPERS_H
#define PYTHON_PORT_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

void micropython_port_vm_hook_loop();
bool micropython_port_should_interrupt();

#ifdef __cplusplus
}
#endif

#endif
