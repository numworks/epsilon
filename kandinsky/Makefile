SFLAGS += -Ikandinsky/include
objs += $(addprefix kandinsky/src/,\
  color.o\
  context.o\
  context_line.o\
  context_pixel.o\
  context_rect.o\
  context_text.o\
  framebuffer.o\
  framebuffer_context.o\
  ion_context.o\
  large_font.o\
  point.o\
  rect.o\
  small_font.o\
  text.o\
)
tests += $(addprefix kandinsky/test/,\
  color.cpp\
  rect.cpp\
)

FREETYPE_PATH := /usr/local/Cellar/freetype/2.6.3
# LIBPNG_PATH is optional. If LIBPNG_PATH is not defined, rasterizer will be
# built w/o PNG support and simply won't output an image of the rasterization
#LIBPNG_PATH := /usr/local/Cellar/libpng/1.6.21

generated_headers += $(addprefix kandinsky/src/, small_font.h large_font.h)

small_font_files = $(addprefix kandinsky/src/, small_font.h small_font.c)
large_font_files = $(addprefix kandinsky/src/, large_font.h large_font.c)

RASTERIZER_CFLAGS := -std=c99 `freetype-config --cflags`
RASTERIZER_LDFLAGS := `freetype-config --libs`

ifdef LIBPNG_PATH
  small_font_files += kandinsky/src/small_font.png
  large_font_files += kandinsky/src/large_font.png
  RASTERIZER_CFLAGS += -I$(LIBPNG_PATH)/include -DGENERATE_PNG=1 -L$(LIBPNG_PATH)/lib -lpng
endif

# Even though raster will generate both .c and .h files, we don't declare it as
# such to make. If we did, "make -jN" with N>1 may call "raster" twice.

kandinsky/src/small_font.h: kandinsky/src/small_font.c
kandinsky/src/small_font.c: kandinsky/fonts/rasterizer
	@echo "RASTER  $(small_font_files)"
	$(Q) $< kandinsky/fonts/SmallSourcePixel.ttf 12 12 SmallFont $(small_font_files)

kandinsky/src/large_font.h: kandinsky/src/large_font.c
kandinsky/src/large_font.c: kandinsky/fonts/rasterizer
	@echo "RASTER  $(large_font_files)"
	$(Q) $< kandinsky/fonts/LargeSourcePixel.ttf 16 16 LargeFont $(large_font_files)

kandinsky/fonts/rasterizer: kandinsky/fonts/rasterizer.c kandinsky/fonts/unicode_for_symbol.c
	@echo "HOSTCC  $@"
	$(Q) $(HOSTCC) $(RASTERIZER_CFLAGS) $^ $(RASTERIZER_LDFLAGS) -o $@

products += $(small_font_files) $(large_font_files) kandinsky/fonts/rasterizer

