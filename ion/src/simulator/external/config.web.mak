# The EM_ASM directive needs to be built using -std=gnu99
SDL_SFLAGS += -std=gnu99

sdl_src += $(addprefix ion/src/simulator/external/sdl/src/, \
  audio/disk/SDL_diskaudio.c \
  audio/emscripten/SDL_emscriptenaudio.c \
  haptic/dummy/SDL_syshaptic.c \
  hidapi/SDL_hidapi.c \
  joystick/emscripten/SDL_sysjoystick.c \
  libm/e_atan2.c \
  libm/e_exp.c \
  libm/e_fmod.c \
  libm/e_log.c \
  libm/e_log10.c \
  libm/e_pow.c \
  libm/e_rem_pio2.c \
  libm/e_sqrt.c \
  libm/k_cos.c \
  libm/k_rem_pio2.c \
  libm/k_sin.c \
  libm/k_tan.c \
  libm/s_atan.c \
  libm/s_copysign.c \
  libm/s_cos.c \
  libm/s_fabs.c \
  libm/s_floor.c \
  libm/s_scalbn.c \
  libm/s_sin.c \
  libm/s_tan.c \
  loadso/dlopen/SDL_sysloadso.c \
  locale/emscripten/SDL_syslocale.c \
  misc/dummy/SDL_sysurl.c \
  thread/generic/SDL_syssem.c \
  thread/generic/SDL_systhread.c \
  thread/generic/SDL_systls.c \
  timer/unix/SDL_systimer.c \
  video/emscripten/SDL_emscriptenevents.c \
  video/emscripten/SDL_emscriptenframebuffer.c \
  video/emscripten/SDL_emscriptenmouse.c \
  video/emscripten/SDL_emscriptenopengles.c \
  video/emscripten/SDL_emscriptenvideo.c \
)
