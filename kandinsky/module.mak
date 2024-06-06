KANDINSKY_codepoints := $(PATH_kandinsky)/fonts/code_points.h

_sources_kandinsky_fonts := $(addprefix fonts/, \
  LargeFont.ttf \
  SmallFont.ttf \
)

_sources_kandinsky_minimal := $(addprefix src/, \
  color.cpp \
  font.cpp \
  point.cpp \
  rect.cpp \
) $(_sources_kandinsky_fonts)

_kandinsky_glyph_index := fonts/codepoint_to_glyph_index.cpp

_sources_kandinsky_extended := $(addprefix src/, \
  context_line.cpp \
  context_pixel.cpp \
  context_rect.cpp \
  context_text.cpp \
  context_circle.cpp \
  framebuffer.cpp \
) $(_kandinsky_glyph_index)

$(call create_module,kandinsky,1, \
  $(_sources_kandinsky_minimal) \
  $(addsuffix :-minimal,$(_sources_kandinsky_extended)) \
)

# Rasterizer

$(call import_module,kandinsky_rasterizer,$(PATH_kandinsky)/fonts)
$(call create_module,kandinsky_rasterizer,1,rasterizer.c)

_cflags_kandinsky_rasterizer := -std=c11 $(shell pkg-config freetype2 --cflags)
_ldflags_kandinsky_rasterizer := $(shell pkg-config freetype2 --libs)

_has_libpng := $(shell pkg-config libpng --exists && echo 1)
ifeq ($(_has_libpng),1)
  _cflags_kandinsky_rasterizer += $(shell pkg-config libpng --cflags) -DGENERATE_PNG=1
  _ldflags_kandinsky_rasterizer += $(shell pkg-config libpng --libs)
endif

_kandinsky_rasterizer := $(TOOLS_DIRECTORY)/rasterizer.bin

$(call create_tool,rasterizer, \
  omg.lz4only \
  kandinsky_rasterizer \
)

PRIORITY_TARGETS_kandinsky += $(addprefix $(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/,$(addsuffix .h,$(basename $(_sources_kandinsky_fonts))))

$(_kandinsky_rasterizer): TOOLS_CFLAGS += $(_cflags_kandinsky_rasterizer)
$(_kandinsky_rasterizer): TOOLS_LDFLAGS += $(_ldflags_kandinsky_rasterizer)

# Raster ttf into cpp

# raster_font, <name>, <size>, <packed width>, <packed height>
define raster_font
$(eval \
$(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/$1.cpp: $(PATH_kandinsky)/fonts/$1.ttf $(_kandinsky_rasterizer) | $$$$(@D)/.
	$$(call rule_label,RASTER)
	$(QUIET) $(_kandinsky_rasterizer) $$< $2 $2 $3 $4 $1 $$(basename $$@).h $$@ $$(dir $$@)/$(notdir $(_kandinsky_glyph_index)) $$(if $(_has_libpng),$$(basename $$@).png,)

$(addprefix $(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/,codepoint_to_glyph_index.cpp $1.h): $(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/$1.cpp
)
endef

$(call raster_font,SmallFont,12,7,14)
$(call raster_font,LargeFont,16,10,18)
