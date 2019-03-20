#ifndef ION_SDL_PLATFORM_H
#define ION_SDL_PLATFORM_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Those functions should be implemented per-platform.
 * They are defined as C function for easier interop. */

SDL_Texture * IonSDLPlatformLoadImage(SDL_Renderer * renderer, const char * identifier);
char * IonSDLPlatformGetLanguageCode();

#ifdef __cplusplus
}
#endif

#endif
