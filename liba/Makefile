SFLAGS += -Iliba/include

liba/src/external/sqlite/mem5.o: CFLAGS += -w

objs += $(addprefix liba/src/, assert.o errno.o malloc.o memcpy.o memset.o strcmp.o strlen.o external/sqlite/mem5.o)
tests += $(addprefix liba/test/, stdint.c)
