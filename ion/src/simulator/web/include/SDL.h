/* This file is a bit tricky. For some reason, SDL keeps including its config
 * this way: '#include "SDL_config.h"'. The problem with this approach is that
 * it prevents the usual trick of adding a header search path to use our own
 * version of SDL_config.h.
 * To work around this, and since SDL.h itself is always included using angle
 * brackets, we hook directly into SDL.h inclusion. */

// clang-format off

#include "SDL_config.h"
#include_next <SDL.h>

// clang-format on
