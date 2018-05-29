SFLAGS += -Iliba/include

include liba/src/$(ARCH)/Makefile

liba/src/external/sqlite/mem5.o: CFLAGS += -w

objs += $(addprefix liba/src/, \
  assert.o \
  bzero.o \
  ctype.o \
  errno.o \
  fpclassify.o \
  fpclassifyf.o \
  ieee754.o \
  malloc.o \
  memcmp.o \
  memcpy.o \
  memmove.o \
  memset.o \
  nearbyint.o \
  nearbyintf.o \
  strcmp.o \
  strchr.o \
  strlcpy.o \
  strlen.o \
  external/sqlite/mem5.o \
)

objs += $(addprefix liba/src/external/openbsd/, \
  b_exp__D.o \
  b_log__D.o \
  b_tgamma.o \
  e_acosf.o \
  e_acoshf.o \
  e_asinf.o \
  e_atanhf.o \
  e_atan2.o \
  e_atan2f.o \
  e_coshf.o \
  e_expf.o \
  e_fmod.o \
  e_fmodf.o \
  e_lgammaf_r.o \
  e_log10f.o \
  e_log2.o \
  e_logf.o \
  e_powf.o \
  e_rem_pio2f.o \
  e_scalb.o \
  e_sinhf.o \
  e_sqrtf.o \
  k_cosf.o \
  k_rem_pio2f.o \
  k_sinf.o \
  k_tanf.o \
  s_asinhf.o\
  s_atanf.o \
  s_ceilf.o \
  s_copysignf.o \
  s_cosf.o \
  s_erf.o \
  s_expm1f.o\
  s_fabsf.o \
  s_floorf.o \
  s_frexpf.o \
  s_frexp.o \
  s_log1pf.o \
  s_logb.o \
  s_modf.o \
  s_modff.o \
  s_rint.o \
  s_roundf.o \
  s_scalbnf.o \
  s_signgam.o \
  s_sinf.o \
  s_tanf.o \
  s_tanhf.o \
  s_trunc.o \
  s_truncf.o \
  w_lgammaf.o \
)

objs += $(addprefix liba/src/external/openbsd/, \
  e_acos.o \
  e_acosh.o \
  e_asin.o \
  e_atanh.o \
  e_cosh.o \
  e_exp.o \
  e_lgamma_r.o \
  e_log.o \
  e_log10.o \
  e_pow.o \
  e_rem_pio2.o \
  e_sinh.o \
  e_sqrt.o \
  k_cos.o \
  k_rem_pio2.o \
  k_sin.o \
  k_tan.o \
  s_asinh.o \
  s_atan.o \
  s_ceil.o \
  s_copysign.o \
  s_cos.o \
  s_expm1.o \
  s_fabs.o \
  s_floor.o \
  s_log1p.o \
  s_round.o \
  s_scalbn.o \
  s_sin.o \
  s_tan.o \
  s_tanh.o \
  w_lgamma.o \
)

liba/src/external/openbsd/%.o: SFLAGS := -Iliba/src/external/openbsd/include $(SFLAGS)
liba/src/external/openbsd/%.o: CFLAGS += -w

tests += $(addprefix liba/test/, \
  aeabi.c \
  double.c \
  ieee754.c \
  long.c \
  math.c \
  setjmp.c \
  stddef.c \
  stdint.c \
  strlcpy.c \
)
