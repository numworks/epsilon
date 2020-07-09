test_ion_external_flash_read_src += $(addprefix ion/test/$(PLATFORM)/$(MODEL)/, \
  external_flash_read.cpp \
)

test_ion_external_flash_write_src += $(addprefix ion/test/$(PLATFORM)/$(MODEL)/, \
  external_flash_write.cpp \
)

test_ion_external_flash_src += $(test_ion_external_flash_read_src) $(test_ion_external_flash_write_src)
$(call object_for,$(test_ion_external_flash_src)): SFLAGS += $(ION_DEVICE_SFLAGS)
