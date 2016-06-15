SFLAGS += -Ikandinsky/include
objs += $(addprefix kandinsky/src/,\
  drawing_area.o\
  font.o\
  line.o\
  pixel.o\
  rect.o\
  text.o\
  types.o\
)
tests += $(addprefix kandinsky/test/,\
  color.c\
  rect.c\
  set_pixel.c\
)

FREETYPE_PATH := /usr/local/Cellar/freetype/2.6.3
# LIBPNG_PATH is optional. If LIBPNG_PATH is not defined, rasterizer will be
# built w/o PNG support and simply won't output an image of the rasterization
#LIBPNG_PATH := /usr/local/Cellar/libpng/1.6.21

kandinsky/src/text.c: kandinsky/src/font.h

font_files = $(addprefix kandinsky/src/, font.h font.c)

RASTERIZER_CFLAGS := -std=c99 `freetype-config --cflags`
RASTERIZER_LDFLAGS := `freetype-config --libs`

ifdef LIBPNG_PATH
  font_files += kandinsky/src/font.png
  RASTERIZER_FLAGS += -I$(LIBPNG_PATH)/include -DGENERATE_PNG=1 -L$(LIBPNG_PATH)/lib -lpng
endif

# Even though raster will generate both .c and .h files, we don't declare it as
# such to make. If we did, "make -jN" with N>1 may call "raster" twice.

kandinsky/src/font.h: kandinsky/src/font.c
kandinsky/src/font.c: kandinsky/fonts/rasterizer
	@echo "RASTER  $(font_files)"
	@$< kandinsky/fonts/ProggyClean.ttf 16 16 $(font_files)

kandinsky/fonts/rasterizer: kandinsky/fonts/rasterizer.c
	@echo "HOSTCC  $@"
	@$(HOSTCC) $(RASTERIZER_CFLAGS) $< $(RASTERIZER_LDFLAGS) -o $@

products += $(font_files) kandinsky/fonts/rasterizer

