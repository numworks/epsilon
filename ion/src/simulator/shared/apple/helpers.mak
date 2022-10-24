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

# Keys images

define rule_for_asset
simulator_app_deps += $(call simulator_app_resource,$(1))
$(call simulator_app_resource,$(1)): ion/src/simulator/assets/$(1) | $$$$(@D)/.
	$$(call rule_label,COPY)
	$(Q) cp $$^ $$@
endef

$(foreach asset,$(ion_simulator_assets),$(eval $(call rule_for_asset,$(asset))))

# Background image

background_image = $(addprefix $(BUILD_DIR),ion/src/simulator/assets/background.jpg)
$(call simulator_app_resource,background.jpg): $(addprefix $(BUILD_DIR),ion/src/simulator/assets/background.jpg)
	$(call rule_label,COPY)
	$(Q) cp $^ $@
simulator_app_deps += $(call simulator_app_resource,background.jpg)

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
ifeq ($(SIMULATOR_ICON_USE_MASK),1)
	$(Q) convert -background "#FFB734" $< -gravity center -scale 80% -extent 1024x1024 ion/src/simulator/assets/icon_mask.svg -alpha Off -compose CopyOpacity -composite -resize $* $@
else
	$(Q) convert -background "#FFB734" -resize $* $< $@
endif

# Export simulator app dependencies

simulator_app_deps += $(simulator_app_binary)
simulator_app_deps += $(call simulator_app_plist,Info.plist)
