#ifndef ION_TIMING_H
#define ION_TIMING_H

#ifdef __cplusplus
#define EXTERNC extern "C"
namespace Ion {
void usleep(uint32_t us);
#else
#define EXTERNC
#endif

EXTERNC uint64_t millis();

EXTERNC void msleep(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
