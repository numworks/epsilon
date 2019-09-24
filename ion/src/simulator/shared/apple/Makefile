# This file contains all the recipies shared between iOS and macOS.
# The only things that have to be customized per platform are the icons and the
# Info.plist.

SIMULATOR_APP_PATH = $(BUILD_DIR)/app/Payload/Epsilon.app

simulator_app_binary = $(addprefix $(SIMULATOR_APP_PATH)/$(SIMULATOR_APP_BINARY_PATH),$(1))
simulator_app_resource = $(addprefix $(SIMULATOR_APP_PATH)/$(SIMULATOR_APP_RESOURCE_PATH),$(1))
simulator_app_plist = $(addprefix $(SIMULATOR_APP_PATH)/$(SIMULATOR_APP_PLIST_PATH),$(1))

# Epsilon binary

.PHONY: force_remake

$(BUILD_DIR)/%/epsilon.bin: force_remake
	$(Q) echo "MAKE    ARCH=$*"
	$(Q) $(MAKE) ARCH=$*

$(call simulator_app_binary,Epsilon): $(patsubst %,$(BUILD_DIR)/%/epsilon.bin,$(ARCHS)) | $$(@D)/.
	$(call rule_label,LIPO)
	$(Q) $(LIPO) -create $^ -output $@

# Background image

$(call simulator_app_resource,background.jpg): ion/src/simulator/assets/background.jpg | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $^ $@

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

SIMULATOR_APP_DEPS += $(call simulator_app_binary,Epsilon)
SIMULATOR_APP_DEPS += $(call simulator_app_plist,Info.plist)
SIMULATOR_APP_DEPS += $(call simulator_app_resource,background.jpg)
