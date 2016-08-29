SFLAGS += -Iliba/include

liba/src/external/sqlite/mem5.o: CFLAGS += -w

objs += $(addprefix liba/src/, assert.o errno.o malloc.o memcpy.o memset.o powf.o strcmp.o strlen.o external/sqlite/mem5.o)
tests += $(addprefix liba/test/, stdint.c)

# The use of aeabi-rt could be made conditional to an AEABI target.
# In practice we're always using liba on such a target.
objs += $(addprefix liba/src/aeabi-rt/, \
  memclr.o \
  memcpy.o \
)
