# This file is built manually by mimic-ing SDL's Xcode project

SDL_SFLAGS += -fasm-blocks -fstrict-aliasing -fobjc-weak
SDL_SFLAGS += -Iion/src/simulator/external/sdl/src/video/khronos
SDL_SFLAGS += -Iion/src/simulator/external/sdl/src/hidapi/hidapi

sdl_src += $(addprefix ion/src/simulator/external/sdl/src/, \
  audio/coreaudio/SDL_coreaudio.m \
  audio/disk/SDL_diskaudio.c \
  file/cocoa/SDL_rwopsbundlesupport.m \
  filesystem/cocoa/SDL_sysfilesystem.m \
  haptic/darwin/SDL_syshaptic.c \
  hidapi/SDL_hidapi.c \
  locale/macosx/SDL_syslocale.m \
  joystick/darwin/SDL_iokitjoystick.c \
  joystick/iphoneos/SDL_mfijoystick.m \
  loadso/dlopen/SDL_sysloadso.c \
  main/dummy/SDL_dummy_main.c \
  misc/macosx/SDL_sysurl.m \
  power/macosx/SDL_syspower.c \
  render/metal/SDL_render_metal.m \
  thread/pthread/SDL_syscond.c \
  thread/pthread/SDL_sysmutex.c \
  thread/pthread/SDL_syssem.c \
  thread/pthread/SDL_systhread.c \
  thread/pthread/SDL_systls.c \
  timer/unix/SDL_systimer.c \
  video/cocoa/SDL_cocoaclipboard.m \
  video/cocoa/SDL_cocoaevents.m \
  video/cocoa/SDL_cocoakeyboard.m \
  video/cocoa/SDL_cocoamessagebox.m \
  video/cocoa/SDL_cocoametalview.m \
  video/cocoa/SDL_cocoamodes.m \
  video/cocoa/SDL_cocoamouse.m \
  video/cocoa/SDL_cocoamousetap.m \
  video/cocoa/SDL_cocoaopengl.m \
  video/cocoa/SDL_cocoaopengles.m \
  video/cocoa/SDL_cocoashape.m \
  video/cocoa/SDL_cocoavideo.m \
  video/cocoa/SDL_cocoavulkan.m \
  video/cocoa/SDL_cocoawindow.m \
)

LDFLAGS += -weak_framework Metal
LDFLAGS += -weak_framework QuartzCore
LDFLAGS += -framework CoreAudio
LDFLAGS += -framework CoreVideo
LDFLAGS += -framework Cocoa
LDFLAGS += -framework IOKit
LDFLAGS += -framework CoreFoundation
LDFLAGS += -framework Carbon
LDFLAGS += -framework ForceFeedback
LDFLAGS += -framework AudioToolbox
LDFLAGS += -framework GameController
LDFLAGS += -framework CoreHaptics
