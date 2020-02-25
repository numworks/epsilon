#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <SDL.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Those functions should be implemented per-platform.
 * They are defined as C function for easier interop. */

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier);
char * IonSimulatorGetLanguageCode();

#if EPSILON_SDL_SCREEN_ONLY
void IonSimulatorKeyboardKeyDown(int keyNumber);
void IonSimulatorKeyboardKeyUp(int keyNumber);
void IonSimulatorEventsPushEvent(int eventNumber);
#endif

bool IonSimulatorSDLKeyDetectedByScan(SDL_Scancode key);
void IonSimulatorCallbackDidRefresh();
void IonSimulatorCallbackDidScanKeyboard();

#ifdef __cplusplus
}
#endif

#endif
