FREETYPE_PATH := /usr/local/Cellar/freetype/2.5.5
LIBPNG_PATH = /usr/local/Cellar/libpng/1.6.17
#LIBPNG_PATH is optional, PNGs won't be generated

CC := clang
CFLAGS := -I$(FREETYPE_PATH)/include/freetype2
LDFLAGS := -L$(FREETYPE_PATH)/lib -lfreetype
ifdef LIBPNG_PATH
CFLAGS += -I$(LIBPNG_PATH)/include -DGENERATE_PNG=1
LDFLAGS += -L$(LIBPNG_PATH)/lib -lpng
endif

default:
	$(CC) $(CFLAGS) $(LDFLAGS) rasterizer.c -o rasterizer

clean:
	rm rasterizer
