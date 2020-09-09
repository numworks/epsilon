# This file contains all the recipies shared between iOS and macOS.
# The only things that have to be customized per platform are the icons and the
# Info.plist.

SIMULATOR_APP_PATH = $(BUILD_DIR)

simulator_app_binary = $(SIMULATOR_APP_PATH)/%.app/$(SIMULATOR_APP_BINARY_PATH)Epsilon
simulator_app_resource = $(SIMULATOR_APP_PATH)/%.app/$(SIMULATOR_APP_RESOURCE_PATH)$(1)
simulator_app_plist = $(SIMULATOR_APP_PATH)/%.app/$(SIMULATOR_APP_PLIST_PATH)$(1)

# Epsilon binary

$(simulator_app_binary): $(foreach arch,$(ARCHS),$(BUILD_DIR)/$(arch)/%.bin) | $$(@D)/.
	$(call rule_label,LIPO)
	$(Q) $(LIPO) -create $^ -output $@

# Background & Keys images

define rule_for_jpg_asset
simulator_app_deps += $(call simulator_app_resource,$(1).jpg)
$(call simulator_app_resource,$(1).jpg): ion/src/simulator/assets/$(1).jpg | $$$$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $$^ $$@
endef

JPG_ASSETS = background horizontal_arrow large_squircle round small_squircle vertical_arrow
$(foreach ASSET,$(JPG_ASSETS),$(eval $(call rule_for_jpg_asset,$(ASSET))))

# Process icons

ifndef SIMULATOR_ICON_SIZES
  $(error SIMULATOR_ICON_SIZES should be defined)
endif

ifndef SIMULATOR_ICONSET
  $(error SIMULATOR_ICONSET should be defined)
endif

SIMULATOR_ICONS = $(addprefix $(SIMULATOR_ICONSET)/,$(addsuffix .png,$(addprefix icon_, $(SIMULATOR_ICON_SIZES))))

$(addprefix $(SIMULATOR_ICONSET)/,icon_%.png): ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" -resize $* $< $@

# Export simulator app dependencies

simulator_app_deps += $(simulator_app_binary)
simulator_app_deps += $(call simulator_app_plist,Info.plist)