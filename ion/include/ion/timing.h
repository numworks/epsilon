#ifndef ION_TIMING_H
#define ION_TIMING_H

#ifdef __cplusplus
#define EXTERNC extern "C"
namespace Ion {
void usleep(long us);
#else
#define EXTERNC
#endif

EXTERNC uint32_t millis();

EXTERNC void msleep(long ms);

#ifdef __cplusplus
}
#endif

#endif
