TEST_EXT_FLASH_REPROGRAM ?= 0
SFLAGS += -DTEST_EXT_FLASH_REPROGRAM=$(TEST_EXT_FLASH_REPROGRAM)
test_ion_external_flash_src += ion/test/$(PLATFORM)/$(MODEL)/external_flash.cpp
$(call object_for,$(test_ion_external_flash_src)): SFLAGS += $(ION_DEVICE_SFLAGS)
