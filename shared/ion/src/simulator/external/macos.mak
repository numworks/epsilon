SOURCES_sdl += $(addprefix $(PATH_sdl)/sdl/src/, \
  audio/coreaudio/SDL_coreaudio.m \
  audio/disk/SDL_diskaudio.c \
  core/unix/SDL_poll.c \
  file/cocoa/SDL_rwopsbundlesupport.m \
  filesystem/cocoa/SDL_sysfilesystem.m \
  haptic/darwin/SDL_syshaptic.c \
  hidapi/SDL_hidapi.c \
  locale/macosx/SDL_syslocale.m \
  joystick/darwin/SDL_iokitjoystick.c \
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

PRIVATE_SFLAGS_sdl += \
  -I$(PATH_sdl)/sdl/src/hidapi/hidapi \
  -I$(PATH_sdl)/sdl/src/video/khronos \
  -fasm-blocks \
  -fobjc-weak \
  -fstrict-aliasing

LDFLAGS_sdl += \
  -weak_framework Metal \
  -weak_framework QuartzCore \
  -framework AudioToolbox \
  -framework Carbon \
  -framework Cocoa \
  -framework CoreAudio \
  -framework CoreFoundation \
  -framework CoreVideo \
  -framework ForceFeedback \
  -framework IOKit \
