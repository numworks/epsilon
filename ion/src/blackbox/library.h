// Turn Epsilon into a library

#include <kandinsky.h>

#ifndef EPSILON_LIB_PREFIX
#error EPSILON_LIB_PREFIX must be defined
#endif

#define CAT_I(a,b) a ## _epsilon_ ## b
#define CAT(a,b) CAT_I(a, b)
#define PREFIXED(name) CAT(EPSILON_LIB_PREFIX, name)

extern "C" {

void PREFIXED(main)();
void PREFIXED(send_event)(int c);
void PREFIXED(wait_event_processed)();
const KDColor * PREFIXED(frame_buffer)();
void PREFIXED(write_frame_buffer_to_file)(const char * c);

}
