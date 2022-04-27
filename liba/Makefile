SFLAGS += -Iliba/include

liba_src += $(addprefix liba/src/, \
  armv7m/setjmp.s \
  armv7m/longjmp.s \
  abs.c \
  assert.c \
  bzero.c \
  ctype.c \
  errno.c \
  fpclassify.c \
  fpclassifyf.c \
  ieee754.c \
  malloc.c \
  memcmp.c \
  memcpy.c \
  memmove.c \
  memset.c \
  nearbyint.c \
  nearbyintf.c \
  strcmp.c \
  strchr.c \
  strlcat.c \
  strlcpy.c \
  strlen.c \
  external/sqlite/mem5.c \
  itoa.c \
)

liba_src += $(addprefix liba/src/external/openbsd/, \
  b_exp__D.c \
  b_log__D.c \
  b_tgamma.c \
  e_acosf.c \
  e_acoshf.c \
  e_asinf.c \
  e_atanhf.c \
  e_atan2f.c \
  e_coshf.c \
  e_expf.c \
  e_fmodf.c \
  e_hypotf.c \
  e_lgammaf_r.c \
  e_log10f.c \
  e_log2.c \
  e_logf.c \
  e_powf.c \
  e_rem_pio2f.c \
  e_scalb.c \
  e_sinhf.c \
  e_sqrtf.c \
  k_cosf.c \
  k_rem_pio2f.c \
  k_sinf.c \
  k_tanf.c \
  s_asinhf.o\
  s_atanf.c \
  s_ceilf.c \
  s_copysignf.c \
  s_cosf.c \
  s_erf.c \
  s_erff.c \
  s_expm1f.o\
  s_fabsf.c \
  s_fmaxf.c \
  s_floorf.c \
  s_frexpf.c \
  s_frexp.c \
  s_log1pf.c \
  s_logb.c \
  s_logbf.c \
  s_modf.c \
  s_modff.c \
  s_nextafter.c \
  s_nextafterf.c \
  s_rint.c \
  s_roundf.c \
  s_scalbnf.c \
  s_signgam.c \
  s_sinf.c \
  s_tanf.c \
  s_tanhf.c \
  s_trunc.c \
  s_truncf.c \
  w_lgammaf.c \
)

liba_src += $(addprefix liba/src/external/openbsd/, \
  e_acos.c \
  e_acosh.c \
  e_asin.c \
  e_atanh.c \
  e_atan2.c \
  e_cosh.c \
  e_exp.c \
  e_fmod.c \
  e_hypot.c \
  e_lgamma_r.c \
  e_log.c \
  e_log10.c \
  e_pow.c \
  e_rem_pio2.c \
  e_sinh.c \
  e_sqrt.c \
  k_cos.c \
  k_rem_pio2.c \
  k_sin.c \
  k_tan.c \
  s_asinh.c \
  s_atan.c \
  s_ceil.c \
  s_copysign.c \
  s_cos.c \
  s_expm1.c \
  s_fabs.c \
  s_fmax.c \
  s_floor.c \
  s_log1p.c \
  s_round.c \
  s_scalbn.c \
  s_sin.c \
  s_tan.c \
  s_tanh.c \
  w_lgamma.c \
)

$(call object_for,liba/src/external/sqlite/mem5.c): CFLAGS += -w
$(call object_for,liba/src/external/openbsd/%.c): SFLAGS := -Iliba/src/external/openbsd/include $(SFLAGS)
$(call object_for,liba/src/external/openbsd/%.c): CFLAGS += -w

tests_src += $(addprefix liba/test/, \
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

# The use of aeabi-rt could be made conditional to an AEABI target.
# In practice we're always using liba on such a target.
liba_src += $(addprefix liba/src/aeabi-rt/, \
  atexit.c \
)
