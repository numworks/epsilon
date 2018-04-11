#ifndef PYTHON_PORT_HELPERS_H
#define PYTHON_PORT_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

/* should_interrupt effectively does something once every 20000 calls. It checks
 * if a key is down to raise an interruption flag. */
void micropython_port_should_interrupt();

#ifdef __cplusplus
}
#endif

#endif
