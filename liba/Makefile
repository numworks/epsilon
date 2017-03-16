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
  a_atanf.o \
  e_acosf.o \
  e_acoshf.o \
  e_asinf.o \
  e_atanhf.o \
  e_coshf.o \
  e_expf.o \
  e_lgammaf_r.o \
  e_log10f.o \
  e_logf.o \
  e_powf.o \
  e_rem_pio2f.o \
  e_sinhf.o \
  e_sqrtf.o \
  k_cosf.o \
  k_rem_pio2f.o \
  k_sinf.o \
  k_tanf.o \
  s_asinhf.o\
  s_ceilf.o \
  s_copysignf.o \
  s_cosf.o \
  s_expm1f.o\
  s_fabsf.o \
  s_floorf.o \
  s_log1pf.o \
  s_roundf.o \
  s_scalbnf.o \
  s_signgam.o \
  s_sinf.o \
  s_tanf.o \
  s_tanhf.o \
  w_lgammaf.o \
)

liba/src/external/openbsd/%.o: CFLAGS += -Iliba/src/external/openbsd/include -Wno-parentheses -Wno-implicit-function-declaration
# isnanf ans isinff are throwing implicit declaration warnings
ifeq ($(DEBUG),1)
# OpenBSD uses double constants ("0.5" instead of "0.5f") in single-precision
# code. That's annoying because Clang rightfully decides to emit double-to-float
# aeabi conversions when building in -O0 mode, and we really don't want to code
# such functions. A simple workaround is to always build this file -Os.
liba/src/external/openbsd/e_expf.o: CFLAGS += -Os
liba/src/external/openbsd/s_expm1f.o: CFLAGS += -Os
liba/src/external/openbsd/s_log1pf.o: CFLAGS += -Os
liba/src/external/openbsd/s_roundf.o: CFLAGS += -Os
endif

tests += $(addprefix liba/test/, \
  ieee754.c \
  stdint.c \
  strlcpy.c \
)

# The use of aeabi-rt could be made conditional to an AEABI target.
# In practice we're always using liba on such a target.
objs += $(addprefix liba/src/aeabi-rt/, \
  atexit.o \
  memclr.o \
  memcpy.o \
)
