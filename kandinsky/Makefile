SFLAGS += -Ikandinsky/include
objs += $(addprefix kandinsky/src/, font.o line.o pixel.o rect.o text.o)
tests += $(addprefix kandinsky/test/, set_pixel.c)

FREETYPE_PATH := /usr/local/Cellar/freetype/2.5.5
# LIBPNG_PATH is optional. If LIBPNG_PATH is not defined, rasterizer will be
# built w/o PNG support and simply won't output an image of the rasterization
LIBPNG_PATH := /usr/local/Cellar/libpng/1.6.17

kandinsky/src/text.c: kandinsky/src/font.h

font_files = $(addprefix kandinsky/src/, font.h font.c)

RASTERIZER_FLAGS := -I$(FREETYPE_PATH)/include/freetype2 -L$(FREETYPE_PATH)/lib -lfreetype
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
	@clang $(RASTERIZER_FLAGS) $< -o $@

products += $(font_files) kandinsky/fonts/rasterizer

