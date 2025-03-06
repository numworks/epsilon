Q ?= @
CC = arm-none-eabi-gcc
BUILD_DIR = $(OUTPUT_DIR)
NWLINK = npx --yes -- nwlink@0.0.16
LINK_GC = 1
LTO = 1

define object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef

CFLAGS = -std=c99
CFLAGS += $(shell $(NWLINK) eadk-cflags)
CFLAGS += -Os -Wall
CFLAGS += -ggdb
LDFLAGS = -Wl,--relocatable
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs
# LDFLAGS += --specs=nosys.specs # Alternatively, use full-fledged newlib

ifeq ($(LINK_GC),1)
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections
endif

ifeq ($(LTO),1)
CFLAGS += -flto -fno-fat-lto-objects
CFLAGS += -fwhole-program
CFLAGS += -fvisibility=internal
LDFLAGS += -flinker-output=nolto-rel
endif

.PHONY: build
build: $(BUILD_DIR)/$(APP_NAME).nwa

.PHONY: check
check: $(BUILD_DIR)/$(APP_NAME).bin

.PHONY: run
run: $(BUILD_DIR)/$(APP_NAME).nwa src/input.txt
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa --external-data src/input.txt $<

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.nwa src/input.txt
	@echo "BIN     $@"
	$(Q) $(NWLINK) nwa-bin --external-data src/input.txt $< $@

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.nwa src/input.txt
	@echo "ELF     $@"
	$(Q) $(NWLINK) nwa-elf --external-data src/input.txt $< $@

$(BUILD_DIR)/$(APP_NAME).nwa: $(call object_for,$(SOURCES)) $(BUILD_DIR)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(addprefix $(BUILD_DIR)/,%.o): %.c | $(BUILD_DIR)
	@echo "CC      $^"
	$(Q) $(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/icon.o: $(APP_ICON)
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PRECIOUS: $(BUILD_DIR)
$(BUILD_DIR):
	$(Q) mkdir -p $@/src

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR)
