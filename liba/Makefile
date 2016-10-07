SFLAGS += -Iliba/include

liba/src/external/sqlite/mem5.o: CFLAGS += -w

objs += $(addprefix liba/src/, assert.o errno.o malloc.o memcpy.o memset.o strcmp.o strlcpy.o strlen.o external/sqlite/mem5.o)

objs += $(addprefix liba/src/external/msun/, \
  e_log10f.o \
  e_powf.o \
  e_sqrtf.o \
  s_copysignf.o \
  s_fabsf.o \
  s_scalbnf.o \
)

liba/src/external/msun/%.o: CFLAGS += -Iliba/src/external/msun/include -Dlint=1 -Wno-parentheses
liba/src/external/msun/%.o: SFLAGS:=$(filter-out -DDEBUG=1,$(SFLAGS))

tests += $(addprefix liba/test/, stdint.c strlcpy.c)

# The use of aeabi-rt could be made conditional to an AEABI target.
# In practice we're always using liba on such a target.
objs += $(addprefix liba/src/aeabi-rt/, \
  memclr.o \
  memcpy.o \
)
