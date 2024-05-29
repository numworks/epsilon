SOURCES_sdl += $(addprefix $(PATH_sdl)/sdl/src/, \
  audio/coreaudio/SDL_coreaudio.m \
  core/unix/SDL_poll.c \
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

PRIVATE_SFLAGS_sdl += \
  -fasm-blocks \
  -fobjc-arc \
  -fstrict-aliasing

LDFLAGS_sdl += \
  -framework AVFoundation \
  -framework AudioToolbox \
  -framework CoreGraphics \
  -framework CoreHaptics \
  -framework CoreMotion \
  -framework Foundation \
  -framework GameController \
  -framework Metal \
  -framework OpenGLES \
  -framework QuartzCore \
  -framework UIKit
