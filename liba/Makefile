SFLAGS += -Iliba/include

liba/src/external/sqlite/mem5.o: CFLAGS += -w

objs += $(addprefix liba/src/, \
  assert.o \
  errno.o \
  ieee754.o \
  isnanf.o \
  isinff.o \
  malloc.o \
  memcpy.o \
  memset.o \
  strcmp.o \
  strlcpy.o \
  strlen.o \
  external/sqlite/mem5.o \
)

objs += $(addprefix liba/src/external/openbsd/, \
  e_log10f.o \
  e_logf.o \
  e_powf.o \
  e_rem_pio2f.o \
  e_sqrtf.o \
  k_cosf.o \
  k_rem_pio2f.o \
  k_sinf.o \
  k_tanf.o \
  s_copysignf.o \
  s_cosf.o \
  s_fabsf.o \
  s_floorf.o \
  s_scalbnf.o \
  s_sinf.o \
  s_tanf.o \
)

liba/src/external/openbsd/%.o: CFLAGS += -Iliba/src/external/openbsd/include -Wno-parentheses

tests += $(addprefix liba/test/, \
  ieee754.c \
  stdint.c \
  strlcpy.c \
)

# The use of aeabi-rt could be made conditional to an AEABI target.
# In practice we're always using liba on such a target.
objs += $(addprefix liba/src/aeabi-rt/, \
  memclr.o \
  memcpy.o \
)
