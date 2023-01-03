# This file is built manually by mimic-ing SDL's Xcode project

SDL_SFLAGS += -fasm-blocks -fstrict-aliasing -fobjc-arc

sdl_src += $(addprefix ion/src/simulator/external/sdl/src/, \
  audio/coreaudio/SDL_coreaudio.m \
  file/cocoa/SDL_rwopsbundlesupport.m \
  filesystem/cocoa/SDL_sysfilesystem.m \
  haptic/dummy/SDL_syshaptic.c \
  hidapi/ios/hid.m \
  joystick/iphoneos/SDL_mfijoystick.m \
  loadso/dlopen/SDL_sysloadso.c \
  locale/dummy/SDL_syslocale.c \
  main/uikit/SDL_uikit_main.c \
  misc/ios/SDL_sysurl.m \
  power/uikit/SDL_syspower.m \
  render/metal/SDL_render_metal.m \
  sensor/coremotion/SDL_coremotionsensor.m \
  thread/pthread/SDL_syscond.c \
  thread/pthread/SDL_sysmutex.c \
  thread/pthread/SDL_syssem.c \
  thread/pthread/SDL_systhread.c \
  thread/pthread/SDL_systls.c \
  timer/unix/SDL_systimer.c \
  video/uikit/SDL_uikitappdelegate.m \
  video/uikit/SDL_uikitclipboard.m \
  video/uikit/SDL_uikitevents.m \
  video/uikit/SDL_uikitmessagebox.m \
  video/uikit/SDL_uikitmetalview.m \
  video/uikit/SDL_uikitmodes.m \
  video/uikit/SDL_uikitopengles.m \
  video/uikit/SDL_uikitopenglview.m \
  video/uikit/SDL_uikitvideo.m \
  video/uikit/SDL_uikitview.m \
  video/uikit/SDL_uikitviewcontroller.m \
  video/uikit/SDL_uikitvulkan.m \
  video/uikit/SDL_uikitwindow.m \
)

LDFLAGS += -framework AVFoundation
LDFLAGS += -framework AudioToolbox
LDFLAGS += -framework CoreGraphics
LDFLAGS += -framework CoreMotion
LDFLAGS += -framework Foundation
LDFLAGS += -framework GameController
LDFLAGS += -framework Metal
LDFLAGS += -framework OpenGLES
LDFLAGS += -framework QuartzCore
LDFLAGS += -framework UIKit
LDFLAGS += -framework CoreHaptics
