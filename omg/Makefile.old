SFLAGS += -Iomg/include

omg_decompress_src := $(addprefix omg/src/, \
  external/lz4/lz4.c \
  memory_decompress.cpp \
)

omg_src += $(addprefix omg/src/,\
  arithmetic.cpp \
  code_point.cpp\
  directions.cpp \
  float.cpp \
  list.cpp \
  memory.cpp \
  print.cpp \
  unicode_helper.cpp \
  utf8_decoder.cpp\
  utf8_helper.cpp\
) $(omg_decompress_src)

tests_src += $(addprefix omg/test/,\
  bit_helper.cpp\
  print.cpp\
  signaling_nan.cpp\
)

omg_bench_src = $(addprefix omg/src/,\
  code_point.cpp\
  print.cpp \
  utf8_decoder.cpp \
  utf8_helper.cpp \
) $(omg_decompress_src)

omg_kernel_src = $(addprefix omg/src/,\
  utf8_decoder.cpp:+kernelassert \
  utf8_helper.cpp:+kernelassert \
) $(omg_decompress_src)
