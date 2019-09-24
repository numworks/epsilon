# This file is built manually by mimic-ing SDL's Xcode project

SDL_SFLAGS += -fasm-blocks -fstrict-aliasing
SDL_SFLAGS += -Iion/src/simulator/external/sdl/src/video/khronos
SDL_SFLAGS += -Iion/src/simulator/external/sdl/src/hidapi/hidapi

sdl_src += $(addprefix ion/src/simulator/external/sdl/src/, \
  audio/coreaudio/SDL_coreaudio.m \
  audio/disk/SDL_diskaudio.c \
  file/cocoa/SDL_rwopsbundlesupport.m \
  filesystem/cocoa/SDL_sysfilesystem.m \
  haptic/darwin/SDL_syshaptic.c \
  hidapi/mac/hid.c \
  joystick/darwin/SDL_sysjoystick.c \
  loadso/dlopen/SDL_sysloadso.c \
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
  video/x11/SDL_x11clipboard.c \
  video/x11/SDL_x11dyn.c \
  video/x11/SDL_x11events.c \
  video/x11/SDL_x11framebuffer.c \
  video/x11/SDL_x11keyboard.c \
  video/x11/SDL_x11messagebox.c \
  video/x11/SDL_x11modes.c \
  video/x11/SDL_x11mouse.c \
  video/x11/SDL_x11opengl.c \
  video/x11/SDL_x11opengles.c \
  video/x11/SDL_x11shape.c \
  video/x11/SDL_x11touch.c \
  video/x11/SDL_x11video.c \
  video/x11/SDL_x11window.c \
  video/x11/SDL_x11xinput2.c \
  video/x11/imKStoUCS.c \
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
