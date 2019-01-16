SFLAGS += -Ikandinsky/include

src += $(addprefix kandinsky/src/,\
  color.cpp \
  context.cpp \
  context_line.cpp \
  context_pixel.cpp \
  context_rect.cpp \
  context_text.cpp \
  font.cpp \
  framebuffer.cpp \
  framebuffer_context.cpp \
  ion_context.cpp \
  point.cpp \
  rect.cpp \
  unicode/utf8_decoder.cpp\
  unicode/utf8_helper.cpp\
)

src += $(addprefix kandinsky/fonts/, \
  LargeSourcePixel.ttf \
  SmallSourcePixel.ttf \
)

tests += $(addprefix kandinsky/test/,\
  color.cpp\
  font.cpp\
  rect.cpp\
  utf8_decoder.cpp\
)

RASTERIZER_CFLAGS := -std=c99 `pkg-config freetype2 --cflags`
RASTERIZER_LDFLAGS := `pkg-config freetype2 --libs`

ifdef LIBPNG_PATH
  small_font_files += kandinsky/src/small_font.png
  large_font_files += kandinsky/src/large_font.png
  RASTERIZER_CFLAGS += -I$(LIBPNG_PATH)/include -DGENERATE_PNG=1 -L$(LIBPNG_PATH)/lib -lpng
endif

$(eval $(call rule_for, \
  HOSTCC, \
  kandinsky/fonts/rasterizer, \
  kandinsky/fonts/rasterizer.c kandinsky/fonts/unicode_for_symbol.c $(addprefix ion/src/external/lz4/, lz4.c lz4hc.c), \
  $$(HOSTCC) $$(RASTERIZER_CFLAGS) $$^ $$(RASTERIZER_LDFLAGS) -o $$@ \
))

RASTERIZER := $(BUILD_DIR)/kandinsky/fonts/rasterizer

$(eval $(call rule_for, \
  RASTER, \
  kandinsky/fonts/SmallSourcePixel.cpp, \
  kandinsky/fonts/SmallSourcePixel.ttf $$(RASTERIZER), \
  $$(RASTERIZER) $$< 12 12 SmallFont $$@ \
))

$(eval $(call rule_for, \
  RASTER, \
  kandinsky/fonts/LargeSourcePixel.cpp, \
  kandinsky/fonts/LargeSourcePixel.ttf $$(RASTERIZER), \
  $$(RASTERIZER) $$< 16 16 LargeFont $$@ \
))
