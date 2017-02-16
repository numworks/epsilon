#ifndef ION_C_H
#define ION_C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ion_log_print_string(const char * message);
void ion_log_print_integer(uint32_t integer);

#ifdef __cplusplus
}
#endif

#endif
