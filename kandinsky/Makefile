SFLAGS += -Ikandinsky/include -Iplatform/stm32f429
objs += $(addprefix kandinsky/src/, line.o text.o font.o rect.o referential.o)

FREETYPE_PATH := /usr/local/Cellar/freetype/2.5.5
LIBPNG_PATH := /usr/local/Cellar/libpng/1.6.17

kandinsky/src/text.c: kandinsky/src/font.h

font_files = $(addprefix kandinsky/src/, font.h font.c)

# Even though raster will generate both .c and .h files, we don't declare it as
# such to make. If we did, "make -jN" with N>1 may call "raster" twice.

kandinsky/src/font.h: kandinsky/src/font.c
kandinsky/src/font.c: kandinsky/fonts/rasterizer
	@echo "RASTER  $(font_files)"
	@$< kandinsky/fonts/ProggyClean.ttf 16 16 $(font_files)

kandinsky/fonts/rasterizer: kandinsky/fonts/rasterizer.c
	@echo "HOSTCC  $@"
	@clang -I$(FREETYPE_PATH)/include/freetype2 -L$(FREETYPE_PATH)/lib -lfreetype $< -o $@

products += $(font_files) kandinsky/fonts/rasterizer
