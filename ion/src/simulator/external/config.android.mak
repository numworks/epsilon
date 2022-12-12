SDL_SFLAGS += -DGL_GLEXT_PROTOTYPES

sdl_src += $(addprefix ion/src/simulator/external/sdl/src/, \
  audio/android/SDL_androidaudio.c \
  audio/openslES/SDL_openslES.c \
  core/android/SDL_android.c \
  filesystem/android/SDL_sysfilesystem.c \
  haptic/android/SDL_syshaptic.c \
  hidapi/android/hid.cpp \
  joystick/android/SDL_sysjoystick.c \
  loadso/dlopen/SDL_sysloadso.c \
  locale/android/SDL_syslocale.c \
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
SDL_SFLAGS += -I$(NDK_PATH)/sources/android/cpufeatures
$(BUILD_DIR)/app/android/cpu-features.c: $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $(NDK_PATH)/sources/android/cpufeatures/cpu-features.c $@
sdl_src += $(BUILD_DIR)/app/android/cpu-features.c

LDFLAGS += -lGLESv1_CM
LDFLAGS += -lGLESv2
LDFLAGS += -lOpenSLES
LDFLAGS += -landroid
LDFLAGS += -ldl
LDFLAGS += -ljnigraphics
LDFLAGS += -llog
