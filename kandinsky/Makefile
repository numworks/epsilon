SFLAGS += -Ikandinsky/include

kandinsky_minimal_src += $(addprefix kandinsky/src/,\
  color.cpp \
  font.cpp\
  point.cpp \
  rect.cpp \
)

kandinsky_src += $(addprefix kandinsky/src/,\
  color.cpp \
  context_line.cpp \
  context_pixel.cpp \
  context_rect.cpp \
  context_text.cpp \
  context_circle.cpp \
  font.cpp \
  framebuffer.cpp \
  ion_context.cpp \
  point.cpp \
  rect.cpp \
)

kandinsky_fonts_src += $(addprefix kandinsky/fonts/, \
  LargeFont.ttf \
  SmallFont.ttf \
)
kandinsky_src += $(BUILD_DIR)/kandinsky/fonts/codepoint_to_glyph_index.cpp

kandinsky_src += $(kandinsky_fonts_src)
kandinsky_minimal_src += $(kandinsky_fonts_src)

tests_src += $(addprefix kandinsky/test/,\
  color.cpp\
  font.cpp\
  rect.cpp\
)

code_points = kandinsky/fonts/code_points.h

RASTERIZER_CFLAGS := -std=c11 -Iion/include $(shell pkg-config freetype2 --cflags)
RASTERIZER_LDFLAGS := $(shell pkg-config freetype2 --libs)

HAS_LIBPNG := $(shell pkg-config libpng --exists && echo 1)
ifeq ($(HAS_LIBPNG),1)
  RASTERIZER_CFLAGS += $(shell pkg-config libpng --cflags) -DGENERATE_PNG=1
  RASTERIZER_LDFLAGS += $(shell pkg-config libpng --libs)
endif

$(eval $(call rule_for, \
  HOSTCC, \
  kandinsky/fonts/rasterizer, \
  kandinsky/fonts/rasterizer.c kandinsky/fonts/code_points.h $(addprefix ion/src/external/lz4/, lz4.c lz4hc.c), \
  $$(HOSTCC) $$(RASTERIZER_CFLAGS) $$(filter-out %.h,$$^) $$(RASTERIZER_LDFLAGS) -o $$@, \
  global \
))

RASTERIZER := $(BUILD_DIR)/kandinsky/fonts/rasterizer

# Define a rasterizing recipe. Parameters : font name, size, packed_width, packed_height
define raster_font
$(call rule_for, \
  RASTER, \
  kandinsky/fonts/$(1).cpp, \
  kandinsky/fonts/$(1).ttf $$(RASTERIZER), \
  $$(RASTERIZER) $$< $(2) $(2) $(3) $(4) $(1) $$(dir $$@)/$(1).h $$@ $$(dir $$@)/codepoint_to_glyph_index.cpp $(if $(HAS_LIBPNG),$$(basename $$@).png), \
  global \
)
# codepoint_to_glyph_index.cpp is generated as an aside by the rasterizer
# we tell make that it can be built with an empty recipe as long as the font is built
$(BUILD_DIR)/kandinsky/fonts/codepoint_to_glyph_index.cpp: $(BUILD_DIR)/kandinsky/fonts/$(1).cpp
$(BUILD_DIR)/kandinsky/fonts/$(1).h: $(BUILD_DIR)/kandinsky/fonts/$(1).cpp
kandinsky_deps += $(BUILD_DIR)/kandinsky/fonts/$(1).h
endef

$(eval $(call raster_font,SmallFont,12,7,14))
$(eval $(call raster_font,LargeFont,16,10,18))
