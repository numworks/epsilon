SOURCES_sdl += $(addprefix $(PATH_sdl)/sdl/src/, \
  audio/android/SDL_androidaudio.c \
  audio/openslES/SDL_openslES.c \
  core/android/SDL_android.c \
  core/unix/SDL_poll.c \
  filesystem/android/SDL_sysfilesystem.c \
  haptic/android/SDL_syshaptic.c \
  hidapi/android/hid.cpp \
  joystick/android/SDL_sysjoystick.c \
  loadso/dlopen/SDL_sysloadso.c \
  locale/android/SDL_syslocale.c \
  main/dummy/SDL_dummy_main.c \
  misc/android/SDL_sysurl.c \
  power/android/SDL_syspower.c \
  sensor/android/SDL_androidsensor.c \
  thread/pthread/SDL_syscond.c \
  thread/pthread/SDL_sysmutex.c \
  thread/pthread/SDL_syssem.c \
  thread/pthread/SDL_systhread.c \
  thread/pthread/SDL_systls.c \
  timer/unix/SDL_systimer.c \
  video/android/SDL_androidclipboard.c \
  video/android/SDL_androidevents.c \
  video/android/SDL_androidgl.c \
  video/android/SDL_androidkeyboard.c \
  video/android/SDL_androidmessagebox.c \
  video/android/SDL_androidmouse.c \
  video/android/SDL_androidtouch.c \
  video/android/SDL_androidvideo.c \
  video/android/SDL_androidvulkan.c \
  video/android/SDL_androidwindow.c \
)

# Build the android native library cpufeatures
_sources_sdl_cpufeature := app/android/cpu-features.c
$(call objects_foreach_arch,$(_sources_sdl_cpufeature)): | $$(@D)/.
	$(call rule_label,COPY)
	$(QUIET) cp $(NDK_PATH)/sources/android/cpufeatures/cpu-features.c $@
SOURCES_sdl += $(_sources_sdl_cpufeature)

PRIVATE_SFLAGS_sdl += \
  -DGL_GLEXT_PROTOTYPES \
  -I$(NDK_PATH)/sources/android/cpufeatures

LDFLAGS_sdl += \
  -lGLESv1_CM \
  -lGLESv2 \
  -lOpenSLES \
  -landroid \
  -ldl \
  -ljnigraphics \
  -llog
