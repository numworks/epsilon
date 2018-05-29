liba/src/external/softfloat/src/%.o: CFLAGS += -Iliba/src/external/softfloat/include -Iliba/src/external/softfloat/src/8086 -Iliba/src/external/softfloat/port
liba/src/external/softfloat/src/s_roundPackToF64.o: CFLAGS += -w
liba/src/external/softfloat/src/s_roundPackToF32.o: CFLAGS += -w
# s_roundPackToF64 and s_roundPackToF32 are throwing warnings

objs += $(addprefix liba/src/external/softfloat/src/, \
  8086/s_commonNaNToF32UI.o \
  8086/s_commonNaNToF64UI.o \
  8086/s_f32UIToCommonNaN.o \
  8086/s_f64UIToCommonNaN.o \
  8086/s_propagateNaNF64UI.o \
  8086/softfloat_raiseFlags.o \
  f32_to_f64.o \
  f32_to_i64_r_minMag.o\
  f64_add.o \
  f64_div.o \
  f64_eq.o \
  f64_le.o \
  f64_lt.o \
  f64_mul.o \
  f64_sub.o \
  f64_to_i32_r_minMag.o \
  f64_to_i64_r_minMag.o \
  f64_to_f32.o \
  i32_to_f64.o \
  i64_to_f32.o \
  i64_to_f64.o \
  s_addMagsF64.o \
  s_approxRecip32_1.o \
  s_approxRecip_1Ks.o \
  s_countLeadingZeros32.o \
  s_countLeadingZeros64.o \
  s_countLeadingZeros8.o \
  s_mul64To128M.o \
  s_normRoundPackToF64.o \
  s_normSubnormalF32Sig.o \
  s_normSubnormalF64Sig.o \
  s_roundPackToF32.o \
  s_roundPackToF64.o \
  s_shiftRightJam32.o \
  s_shiftRightJam64.o \
  s_shortShiftRightJam64.o \
  s_subMagsF64.o \
  softfloat_state.o \
  ui32_to_f64.o \
  ui64_to_f64.o \
)
