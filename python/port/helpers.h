#ifndef PYTHON_PORT_HELPERS_H
#define PYTHON_PORT_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void micropython_port_vm_hook_loop();
bool micropython_port_should_interrupt();
void micropython_port_interruptible_msleep(uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif
