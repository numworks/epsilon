SFLAGS += -Ikandinsky/include

kandinsky_src += $(addprefix kandinsky/src/,\
  color.cpp \
  context.cpp \
  context_circle.cpp \
  context_line.cpp \
  context_pixel.cpp \
  context_polygon.cpp \
  context_rect.cpp \
  context_text.cpp \
  font.cpp \
  framebuffer.cpp \
  framebuffer_context.cpp \
  ion_context.cpp \
  point.cpp \
  postprocess_context.cpp \
  postprocess_gamma_context.cpp \
  postprocess_invert_context.cpp \
  postprocess_zoom_context.cpp \
  rect.cpp \
)

simple_kandinsky_src := $(kandinsky_src)
default_kandinsky_src := $(kandinsky_src)

tests_src += $(addprefix kandinsky/test/,\
  color.cpp\
  font.cpp\
  rect.cpp\
)

code_points = kandinsky/fonts/code_points.h

RASTERIZER_CFLAGS := -std=c99 $(shell pkg-config freetype2 --cflags)
RASTERIZER_LDFLAGS := $(shell pkg-config freetype2 --libs)

HAS_LIBPNG := $(shell pkg-config libpng --exists && echo 1)
ifeq ($(HAS_LIBPNG),1)
  RASTERIZER_CFLAGS += $(shell pkg-config libpng --cflags) -DGENERATE_PNG=1
  RASTERIZER_LDFLAGS += $(shell pkg-config libpng --libs)
endif

$(eval $(call rule_for, \
  HOSTCC, \
  kandinsky/fonts/rasterizer, \
  kandinsky/fonts/rasterizer.c $(addprefix ion/src/external/lz4/, lz4.c lz4hc.c), \
  $$(HOSTCC) $$(RASTERIZER_CFLAGS) $$^ $$(RASTERIZER_LDFLAGS) -o $$@, \
  global \
))

RASTERIZER := $(BUILD_DIR)/kandinsky/fonts/rasterizer

# Define a rasterizing recipe. Parameters : font source, font name, size, packed_width, packed_height
define raster_font
$(call rule_for, \
  RASTER, \
  kandinsky/fonts/$(2).cpp, \
  kandinsky/fonts/$(1).ttf $$(RASTERIZER), \
  $$(RASTERIZER) $$< $(3) $(4) $(4) $(5) $(6) $(1) $$@ $(if $(HAS_LIBPNG),$$(basename $$@).png), \
  global \
)
endef

ifdef HAS_READER

kandinsky_src += $(addprefix kandinsky/fonts/, \
  LargeFontExtended.ttf \
  SmallFontExtended.ttf \
  LargeFontSimple.ttf \
  SmallFontSimple.ttf \
)

default_kandinsky_src += $(addprefix kandinsky/fonts/, \
  LargeFontExtended.ttf \
  SmallFontExtended.ttf \
)

simple_kandinsky_src += $(addprefix kandinsky/fonts/, \
  LargeFontSimple.ttf \
  SmallFontSimple.ttf \
)

$(eval $(call raster_font,SmallFont,SmallFontExtended,1,12,7,14))
$(eval $(call raster_font,LargeFont,LargeFontExtended,1,16,10,18))

$(eval $(call raster_font,SmallFont,SmallFontSimple,0,12,7,14))
$(eval $(call raster_font,LargeFont,LargeFontSimple,0,16,10,18))

else

kandinsky_src += $(addprefix kandinsky/fonts/, \
  LargeFont.ttf \
  SmallFont.ttf \
)

simple_kandinsky_src = $(kandinsky_src)
default_kandinsky_src = $(kandinsky_src)

$(eval $(call raster_font,SmallFont,SmallFontSimple,0,12,7,14))
$(eval $(call raster_font,LargeFont,LargeFontSimple,0,16,10,18))

endif
