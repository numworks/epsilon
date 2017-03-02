#ifndef ION_LOG_H
#define ION_LOG_H

// Ion offers a C-compatible logging API

#include <stdint.h>

#ifdef DEBUG

#ifdef __cplusplus
extern "C" {
#endif

void ion_log_string(const char * message);
void ion_log_uint32(uint32_t integer);

#ifdef __cplusplus
}
#endif

#else

#define ion_log_print_string(m) ((void)0)
#define ion_log_print_uint32(i) ((void)0)

#endif

#endif
