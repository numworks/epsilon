#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Those functions should be implemented per-platform.
 * They are defined as C function for easier interop. */

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier);
char * IonSimulatorGetLanguageCode();

void IonSimulatorTelemetryInit();
void IonSimulatorTelemetryEvent(const char * eventName);
void IonSimulatorTelemetryDeinit();

#if EPSILON_SDL_SCREEN_ONLY

void IonSimulatorKeyboardKeyDown(int keyNumber);
void IonSimulatorKeyboardKeyUp(int keyNumber);

void IonSimulatorEventsPushEvent(int eventNumber);

#endif

void IonSimulatorCallbackDidRefresh();
void IonSimulatorCallbackDidScanKeyboard();

#ifdef __cplusplus
}
#endif

#endif
