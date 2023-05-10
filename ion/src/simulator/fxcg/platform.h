#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Those functions should be implemented per-platform.
 * They are defined as C function for easier interop. */

const char * IonSimulatorGetLanguageCode();

void IonSimulatorKeyboardKeyDown(int keyNumber);
void IonSimulatorKeyboardKeyUp(int keyNumber);
void IonSimulatorEventsPushEvent(int eventNumber);

#ifdef __cplusplus
}
#endif

#endif