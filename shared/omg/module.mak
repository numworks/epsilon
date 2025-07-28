_sources_omg_decompress := $(addprefix src/, \
  external/lz4/lz4.c \
  memory_decompress.cpp \
)

_sources_omg_print := $(addprefix src/, \
  code_point.cpp \
  print.cpp \
)

_sources_omg_utf8 := $(addprefix src/, \
  utf8_decoder.cpp \
  utf8_helper.cpp \
)

_sources_omg_all := $(addprefix src/, \
  arithmetic.cpp \
  directions.cpp \
  float.cpp \
  list.cpp \
  memory.cpp \
  unicode_helper.cpp \
) $(_sources_omg_decompress) $(_sources_omg_print) $(_sources_omg_utf8)

_sources_omg_lz4only := $(addprefix src/external/lz4/, \
  lz4.c \
  lz4hc.c \
)

_sources_omg_test := $(addprefix test/, \
  arithmetic.cpp \
  bit_helper.cpp \
  memory.cpp \
  print.cpp \
  ring_buffer.cpp \
  signaling_nan.cpp \
  utf8_decoder.cpp \
  utf8_helper.cpp \
)

$(call create_module,omg,1, \
  $(addsuffix :-lz4only, \
    $(addsuffix :-minimal,$(_sources_omg_all)) \
    $(foreach f,decompress print utf8,$(addsuffix :+$f,$(_sources_omg_$f)))) \
  $(foreach f,lz4only test,$(addsuffix :+$f,$(_sources_omg_$f))) \
)
