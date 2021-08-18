HANDY_TARGETS += driver

driver_src = $(ion_device_driver_src) $(liba_driver_src)
driver_obj = $(call flavored_object_for,$(driver_src),)

$(BUILD_DIR)/driver.$(EXE): $(driver_obj)
$(BUILD_DIR)/driver.$(EXE): LDSCRIPT += ion/src/$(PLATFORM)/$(MODEL)/flash.ld
$(BUILD_DIR)/driver.$(EXE): LDDEPS = $(LDSCRIPT)
