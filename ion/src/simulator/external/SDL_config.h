/* This a NumWorks' patch to the default SDL_config */

#ifndef SDL_config_h_
/* define will be done in the next SDL_config.h */

#include_next "SDL_config.h"

/* Disable the joystick on MacOs to avoid useless dependencies */
#if defined(__MACOSX__)
#define SDL_JOYSTICK_DISABLED 1
#undef SDL_JOYSTICK_MFI
#endif

#endif /* SDL_config_h_ */
