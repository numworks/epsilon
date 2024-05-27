_sources_liba_minimal := $(addprefix src/, \
  memcmp.c \
  memcpy.c \
  memmove.c \
  memset.c \
  ssp.c \
  strchr.c \
  strcmp.c \
  strlcat.c \
  strlcpy.c \
  strlen.c \
)

_sources_liba_extended := $(addprefix src/, \
  external/sqlite/mem5.c \
  abs.c \
  bzero.c \
  ctype.c \
  errno.c \
  fpclassify.c \
  fpclassifyf.c \
  ieee754.c \
  malloc.c \
  nearbyint.c \
  nearbyintf.c \
)

_sources_liba_armv7m := $(addprefix src/armv7m/, \
  setjmp.s \
  longjmp.s \
)

_sources_liba_aeabirt := src/aeabi-rt/atexit.c

_sources_liba_openbsd := $(addprefix src/external/openbsd/, \
  b_exp__D.c \
  b_log__D.c \
  b_tgamma.c \
  e_acos.c \
  e_acosf.c \
  e_acosh.c \
  e_acoshf.c \
  e_asin.c \
  e_asinf.c \
  e_atan2.c \
  e_atan2f.c \
  e_atanh.c \
  e_atanhf.c \
  e_cosh.c \
  e_coshf.c \
  e_exp.c \
  e_expf.c \
  e_fmod.c \
  e_fmodf.c \
  e_hypot.c \
  e_hypotf.c \
  e_lgamma_r.c \
  e_lgammaf_r.c \
  e_log.c \
  e_log10.c \
  e_log10f.c \
  e_log2.c \
  e_logf.c \
  e_pow.c \
  e_powf.c \
  e_rem_pio2.c \
  e_rem_pio2f.c \
  e_scalb.c \
  e_sinh.c \
  e_sinhf.c \
  e_sqrt.c \
  e_sqrtf.c \
  k_cos.c \
  k_cosf.c \
  k_rem_pio2.c \
  k_rem_pio2f.c \
  k_sin.c \
  k_sinf.c \
  k_tan.c \
  k_tanf.c \
  s_asinh.c \
  s_asinhf.o\
  s_atan.c \
  s_atanf.c \
  s_ceil.c \
  s_ceilf.c \
  s_copysign.c \
  s_copysignf.c \
  s_cos.c \
  s_cosf.c \
  s_erf.c \
  s_erff.c \
  s_expm1.c \
  s_expm1f.o\
  s_fabs.c \
  s_fabsf.c \
  s_floor.c \
  s_floorf.c \
  s_fmax.c \
  s_fmaxf.c \
  s_frexp.c \
  s_frexpf.c \
  s_log1p.c \
  s_log1pf.c \
  s_logb.c \
  s_logbf.c \
  s_modf.c \
  s_modff.c \
  s_nextafter.c \
  s_nextafterf.c \
  s_rint.c \
  s_round.c \
  s_roundf.c \
  s_scalbn.c \
  s_scalbnf.c \
  s_signgam.c \
  s_sin.c \
  s_sinf.c \
  s_tan.c \
  s_tanf.c \
  s_tanh.c \
  s_tanhf.c \
  s_trunc.c \
  s_truncf.c \
  w_lgamma.c \
  w_lgammaf.c \
)

_sources_liba_test += $(addprefix test/, \
  aeabi.c \
  double.c \
  float.c \
  ieee754.c \
  long.c \
  math.c \
  setjmp.c \
  stddef.c \
  stdint.c \
  strlcpy.c \
)

$(eval $(call create_module,liba,1, \
  $(addsuffix :-bridge, \
    $(_sources_liba_minimal) \
    $(addsuffix :-minimal,$(_sources_liba_extended)) \
    $(foreach f,armv7m aeabirt openbsd test,$(addsuffix :+$f,$(_sources_liba_$f)))) \
  src/bridge.c:+bridge \
))

SFLAGS_liba += -Iliba/include/bridge:+bridge

$(call objects_foreach_arch,liba/src/external/sqlite/mem5.c): CFLAGS += -w
$(call objects_foreach_arch,liba/src/external/openbsd/%.c): PRIORITY_SFLAGS := -Iliba/src/external/openbsd/include
$(call objects_foreach_arch,liba/src/external/openbsd/%.c): CFLAGS += -w
# A few OpenBSD libm files define constants using the "volatile" keyword. Like
# "static const volatile float huge = 1.0e+30;". This obviously doesn't make any
# sense, and has been removed from other libm such as openlibm.
# Luckily the "volatile" keyword is not used anywhere else in those files, so we
# can simply define it as an empty macro.
$(call objects_foreach_arch,$(addprefix liba/src/external/openbsd/,e_expf.c s_expm1f.c e_coshf.c e_powf.c)): CFLAGS += -Dvolatile=
