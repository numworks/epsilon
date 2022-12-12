SDL_SFLAGS += -Iion/src/simulator/external/sdl/src/video/khronos
SDL_SFLAGS += -DHAVE_LIBC

sdl_src += $(addprefix ion/src/simulator/external/sdl/src/, \
  audio/directsound/SDL_directsound.c \
  audio/disk/SDL_diskaudio.c \
  audio/wasapi/SDL_wasapi.c \
  audio/wasapi/SDL_wasapi_win32.c \
  audio/winmm/SDL_winmm.c \
  core/windows/SDL_windows.c \
  core/windows/SDL_xinput.c \
  filesystem/windows/SDL_sysfilesystem.c \
  haptic/windows/SDL_dinputhaptic.c \
  haptic/windows/SDL_windowshaptic.c \
  haptic/windows/SDL_xinputhaptic.c \
  hidapi/windows/hid.c \
  joystick/windows/SDL_dinputjoystick.c \
  joystick/windows/SDL_mmjoystick.c \
  joystick/windows/SDL_windowsjoystick.c \
  joystick/windows/SDL_xinputjoystick.c \
  loadso/windows/SDL_sysloadso.c \
  locale/windows/SDL_syslocale.c \
  main/windows/SDL_windows_main.c \
  misc/windows/SDL_sysurl.c \
  power/windows/SDL_syspower.c \
  render/direct3d/SDL_render_d3d.c \
  render/direct3d/SDL_shaders_d3d.c \
  render/direct3d11/SDL_render_d3d11.c \
  render/direct3d11/SDL_shaders_d3d11.c \
  render/SDL_d3dmath.c \
  thread/generic/SDL_syscond.c \
  thread/windows/SDL_sysmutex.c \
  thread/windows/SDL_syssem.c \
  thread/windows/SDL_systhread.c \
  thread/windows/SDL_systls.c \
  timer/windows/SDL_systimer.c \
  video/windows/SDL_windowsclipboard.c \
  video/windows/SDL_windowsevents.c \
  video/windows/SDL_windowsframebuffer.c \
  video/windows/SDL_windowskeyboard.c \
  video/windows/SDL_windowsmessagebox.c \
  video/windows/SDL_windowsmodes.c \
  video/windows/SDL_windowsmouse.c \
  video/windows/SDL_windowsopengl.c \
  video/windows/SDL_windowsopengles.c \
  video/windows/SDL_windowsshape.c \
  video/windows/SDL_windowsvideo.c \
  video/windows/SDL_windowsvulkan.c \
  video/windows/SDL_windowswindow.c \
)

LDFLAGS += -lgdi32
LDFLAGS += -limm32
LDFLAGS += -lole32
LDFLAGS += -loleaut32
LDFLAGS += -lsetupapi
LDFLAGS += -lshell32
LDFLAGS += -luser32
LDFLAGS += -lversion
LDFLAGS += -lwinmm
LDFLAGS += -static-libgcc
