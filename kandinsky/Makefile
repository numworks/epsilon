SFLAGS += -Ikandinsky/include
objs += $(addprefix kandinsky/src/,\
  color.o\
  context.o\
  context_line.o\
  context_pixel.o\
  context_rect.o\
  context_text.o\
  font.o\
  font_large.o\
  font_small.o\
  framebuffer.o\
  framebuffer_context.o\
  ion_context.o\
  point.o\
  rect.o\
)

tests += $(addprefix kandinsky/test/,\
  color.cpp\
  rect.cpp\
)

FREETYPE_PATH := /usr/local/Cellar/freetype/2.6.3
# LIBPNG_PATH is optional. If LIBPNG_PATH is not defined, rasterizer will be
# built w/o PNG support and simply won't output an image of the rasterization
#LIBPNG_PATH := /usr/local/Cellar/libpng/1.6.21

small_font_files = $(addprefix kandinsky/src/, font_small.cpp)
large_font_files = $(addprefix kandinsky/src/, font_large.cpp)

RASTERIZER_CFLAGS := -std=c99 `pkg-config freetype2 --cflags`
RASTERIZER_LDFLAGS := `pkg-config freetype2 --libs`

ifdef LIBPNG_PATH
  small_font_files += kandinsky/src/small_font.png
  large_font_files += kandinsky/src/large_font.png
  RASTERIZER_CFLAGS += -I$(LIBPNG_PATH)/include -DGENERATE_PNG=1 -L$(LIBPNG_PATH)/lib -lpng
endif

# Even though raster will generate both .c and .h files, we don't declare it as
# such to make. If we did, "make -jN" with N>1 may call "raster" twice.

kandinsky/src/font_small.cpp: kandinsky/fonts/rasterizer
	@echo "RASTER  $(small_font_files)"
	$(Q) $< kandinsky/fonts/SmallSourcePixel.ttf 12 12 SmallFont $(small_font_files)

kandinsky/src/font_large.cpp: kandinsky/fonts/rasterizer
	@echo "RASTER  $(large_font_files)"
	$(Q) $< kandinsky/fonts/LargeSourcePixel.ttf 16 16 LargeFont $(large_font_files)

kandinsky/fonts/rasterizer: kandinsky/fonts/rasterizer.c kandinsky/fonts/unicode_for_symbol.c $(addprefix ion/src/external/lz4/, lz4.c lz4hc.c)
	@echo "HOSTCC  $@"
	$(Q) $(HOSTCC) $(RASTERIZER_CFLAGS) $^ $(RASTERIZER_LDFLAGS) -o $@

products += $(small_font_files) $(large_font_files) kandinsky/fonts/rasterizer

