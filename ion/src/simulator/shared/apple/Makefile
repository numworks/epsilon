# This file contains all the recipies shared between iOS and macOS.
# The only things that have to be customized per platform are the icons and the
# Info.plist.

SIMULATOR_APP_PATH = $(BUILD_DIR)/app/Payload

simulator_app_binary = $(addprefix $(SIMULATOR_APP_PATH)/$(1).app/$(SIMULATOR_APP_BINARY_PATH),$(2))
simulator_app_resource = $(addprefix $(SIMULATOR_APP_PATH)/$(1).app/$(SIMULATOR_APP_RESOURCE_PATH),$(2))
simulator_app_plist = $(addprefix $(SIMULATOR_APP_PATH)/$(1).app/$(SIMULATOR_APP_PLIST_PATH),$(2))

# Epsilon binary

.PHONY: force_remake

define rule_for_epsilon
$$(BUILD_DIR)/%/$(1): force_remake
	$(Q) echo "MAKE    ARCH=$$*"
	$(Q) $$(MAKE) ARCH=$$* $(1)
endef

$(eval $(call rule_for_epsilon,epsilon.bin))
$(eval $(call rule_for_epsilon,epsilon.official.bin))

define rule_for_lipo
$$(call simulator_app_binary,$1,Epsilon): $$(patsubst %,$(BUILD_DIR)/%/$2.bin,$$(ARCHS)) | $$$$(@D)/.
	$$(call rule_label,LIPO)
	$(Q) $$(LIPO) -create $$^ -output $$@
endef

$(eval $(call rule_for_lipo,Epsilon,epsilon))
$(eval $(call rule_for_lipo,Epsilon.official,epsilon.official))

# Background image

$(call simulator_app_resource,%,background.jpg): ion/src/simulator/assets/background.jpg | $$(@D)/.
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

simulator_app_deps += $(call simulator_app_binary,$(1),Epsilon)
simulator_app_deps += $(call simulator_app_plist,$(1),Info.plist)
simulator_app_deps += $(call simulator_app_resource,$(1),background.jpg)
