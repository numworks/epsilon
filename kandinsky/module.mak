$(call assert_defined,KANDINSKY_font_variant)

KANDINSKY_codepoints := $(PATH_kandinsky)/fonts/code_points.h

_sources_kandinsky_fonts := LargeFont.ttf SmallFont.ttf

KANDINSKY_fonts_dependencies := $(patsubst %.ttf,$(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/%.h,$(_sources_kandinsky_fonts))

_sources_kandinsky_minimal := $(addprefix src/, \
  color.cpp \
  font.cpp \
  point.cpp \
  rect.cpp \
) $(addprefix fonts/$(KANDINSKY_font_variant)/,$(_sources_kandinsky_fonts))

_kandinsky_glyph_index := fonts/codepoint_to_glyph_index.cpp

_sources_kandinsky_extended := $(addprefix src/, \
  context_line.cpp \
  context_pixel.cpp \
  context_rect.cpp \
  context_text.cpp \
  context_circle.cpp \
  framebuffer.cpp \
) $(_kandinsky_glyph_index)

_sources_kandinsky_test := $(addprefix test/, \
  color.cpp \
  font.cpp \
  rect.cpp \
)

$(call create_module,kandinsky,1, \
  $(_sources_kandinsky_minimal) \
  $(addsuffix :-minimal,$(_sources_kandinsky_extended)) \
  $(addsuffix :+test,$(_sources_kandinsky_test)) \
)

$(call all_objects_for,$(SOURCES_kandinsky)): $(KANDINSKY_fonts_dependencies)
# FIXME Tests in OMG should not require kandinsky
$(call assert_defined,PATH_omg)
$(call all_objects_for,$(PATH_omg)/test/print.cpp): $(KANDINSKY_fonts_dependencies)

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

$(_kandinsky_rasterizer): TOOLS_CFLAGS += $(_cflags_kandinsky_rasterizer)
$(_kandinsky_rasterizer): TOOLS_LDFLAGS += $(_ldflags_kandinsky_rasterizer)

# Raster ttf into cpp

# raster_font, <name>, <size>, <packed width>, <packed height>
define raster_font
$(eval \
$(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/$(KANDINSKY_font_variant)/$1.cpp: $(PATH_kandinsky)/fonts/$(KANDINSKY_font_variant)/$1.ttf $(_kandinsky_rasterizer) | $$$$(@D)/.
	$$(call rule_label,RASTER)
	$(_kandinsky_rasterizer) \
		$$< \
		$2 $2 $3 $4 \
		$1 \
		$(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/$1.h \
		$$@ \
		$(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/$(notdir $(_kandinsky_glyph_index)) \
		$$(if $(_has_libpng),$$(basename $$@).png,)

$(addprefix $(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/,codepoint_to_glyph_index.cpp $1.h): $(OUTPUT_DIRECTORY)/$(PATH_kandinsky)/fonts/$(KANDINSKY_font_variant)/$1.cpp
)
endef

ifeq ($(KANDINSKY_font_variant),epsilon)
SFLAGS_kandinsky += -DKANDINSKY_FONT_LARGE_FONT=1
$(call raster_font,SmallFont,12,7,14)
$(call raster_font,LargeFont,16,10,18)
endif
ifeq ($(KANDINSKY_font_variant),scandium)
SFLAGS += -DKANDINSKY_FONT_VARIABLE_WIDTH=1
$(call raster_font,SmallFont,12,8,13)
$(call raster_font,LargeFont,16,10,18)
endif

