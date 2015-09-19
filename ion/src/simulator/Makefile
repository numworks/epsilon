objs += $(addprefix ion/src/simulator/, init.o)
objs += $(addprefix ion/src/simulator/boot/, main.o)
objs += $(addprefix ion/src/simulator/display/, fltklcd.o)
objs += $(addprefix ion/src/simulator/keyboard/, fltkkbd.o)

#SFLAGS += -I/usr/local/Cellar/fltk/1.3.3/include -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
LDFLAGS += -L/usr/local/Cellar/fltk/1.3.3/lib -lfltk -lpthread -framework Cocoa
