# Define a standard rule helper
# If passed a last parameter value of with_local_version, we also define an
# extra rule that can build source files within the $(BUILD_DIR). This is useful
# for rules that can be applied for intermediate objects (for example, when
# going .png -> .cpp -> .o).

define rule_label
@ echo "$(shell printf "%-8s" $(strip $(1)))$(@:$(BUILD_DIR)/%=%)"
endef

define rule_for
ifeq ($(strip $(5)),with_local_version)
$(addprefix $$(BUILD_DIR)/,$(strip $(2))): $(addprefix $$(BUILD_DIR)/,$(strip $(3)))
	@ echo "$(shell printf "%-8s" $(strip $(1)))$$(@:$$(BUILD_DIR)/%=%)"
	$(Q) $(4)
endif
$(addprefix $$(BUILD_DIR)/,$(strip $(2))): $(strip $(3)) | $$$$(@D)/.
	@ echo "$(shell printf "%-8s" $(strip $(1)))$$(@:$$(BUILD_DIR)/%=%)"
	$(Q) $(4)
endef

# Helper functions to work with variants

define direct_object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef

# Objects for source files in $(1) matching flavor $(2). A flavor is a dot
# separated list of variants (e.g. large.speed).
define flavored_object_for
$(call direct_object_for,$(call filter_variants,$(1),$(sort $(subst ., ,$(2)))))
endef

define object_for
$(call direct_object_for,$(call any_variant,$(1)))
endef
