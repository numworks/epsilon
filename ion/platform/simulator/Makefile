objs += $(addprefix ion/platform/simulator/, init.o keyboard.o platform.o)
objs += $(addprefix ion/drivers/, fltklcd/fltklcd.o fltkkbd/fltkkbd.o fltkkbd/fltkkbdbutton.o)

#SFLAGS += -I/usr/local/Cellar/fltk/1.3.3/include -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
LDFLAGS += -L/usr/local/Cellar/fltk/1.3.3/lib -lfltk -lpthread -framework Cocoa
