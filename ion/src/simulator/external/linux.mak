SOURCES_sdl += $(addprefix $(PATH_sdl)/sdl/src/, \
  audio/dummy/SDL_dummyaudio.c \
  core/linux/SDL_threadprio.c \
  core/unix/SDL_poll.c \
  file/SDL_rwops.c \
  filesystem/unix/SDL_sysfilesystem.c \
  haptic/dummy/SDL_syshaptic.c \
  hidapi/SDL_hidapi.c \
  joystick/linux/SDL_sysjoystick.c \
  loadso/dlopen/SDL_sysloadso.c \
  locale/unix/SDL_syslocale.c \
  main/dummy/SDL_dummy_main.c \
  misc/dummy/SDL_sysurl.c \
  power/linux/SDL_syspower.c \
  render/opengl/SDL_render_gl.c \
  render/opengl/SDL_shaders_gl.c \
  thread/pthread/SDL_syscond.c \
  thread/pthread/SDL_sysmutex.c \
  thread/pthread/SDL_syssem.c \
  thread/pthread/SDL_systhread.c \
  thread/pthread/SDL_systls.c \
  timer/unix/SDL_systimer.c \
  video/x11/SDL_x11clipboard.c \
  video/x11/SDL_x11dyn.c \
  video/x11/SDL_x11events.c \
  video/x11/SDL_x11framebuffer.c \
  video/x11/SDL_x11keyboard.c \
  video/x11/SDL_x11messagebox.c \
  video/x11/SDL_x11modes.c \
  video/x11/SDL_x11mouse.c \
  video/x11/SDL_x11opengl.c \
  video/x11/SDL_x11shape.c \
  video/x11/SDL_x11touch.c \
  video/x11/SDL_x11video.c \
  video/x11/SDL_x11window.c \
  video/x11/SDL_x11xinput2.c \
  video/x11/imKStoUCS.c \
)

PRIVATE_SFLAGS_sdl += \
  -DHAVE_LIBC \
  -I$(PATH_sdl)/sdl/src/video/khronos

LDFLAGS_sdl += \
  -lX11 \
  -lXext \
  -ldl \
  -lpthread \
  -static-libgcc
