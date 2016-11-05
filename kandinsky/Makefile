SFLAGS += -Ikandinsky/include
objs += $(addprefix kandinsky/src/,\
  color.o\
  context.o\
  context_line.o\
  context_pixel.o\
  context_rect.o\
  context_text.o\
  font.o\
  framebuffer.o\
  framebuffer_context.o\
  ion_context.o\
  point.o\
  rect.o\
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

kandinsky/src/text.cpp: kandinsky/src/font.h
kandinsky/src/context_text.cpp: kandinsky/src/font.h

font_files = $(addprefix kandinsky/src/, font.h font.c)

RASTERIZER_CFLAGS := -std=c99 `freetype-config --cflags`
RASTERIZER_LDFLAGS := `freetype-config --libs`

ifdef LIBPNG_PATH
  font_files += kandinsky/src/font.png
  RASTERIZER_CFLAGS += -I$(LIBPNG_PATH)/include -DGENERATE_PNG=1 -L$(LIBPNG_PATH)/lib -lpng
endif

# Even though raster will generate both .c and .h files, we don't declare it as
# such to make. If we did, "make -jN" with N>1 may call "raster" twice.

kandinsky/src/font.h: kandinsky/src/font.c
kandinsky/src/font.c: kandinsky/fonts/rasterizer
	@echo "RASTER  $(font_files)"
	@$< kandinsky/fonts/ProggyClean.ttf 16 16 $(font_files)

kandinsky/fonts/rasterizer: kandinsky/fonts/rasterizer.c
	@echo "HOSTCC  $@"
	@$(HOSTCC) -std=c99 $(RASTERIZER_CFLAGS) $< $(RASTERIZER_LDFLAGS) -o $@

products += $(font_files) kandinsky/fonts/rasterizer

