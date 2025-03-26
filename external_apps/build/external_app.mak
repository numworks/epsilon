# Configuration
Q ?= @
PLATFORM ?= web

# Verbose
ifeq ("$(origin V)", "command line")
  ifeq ($(V),1)
    Q=
  endif
endif

# Host detection
ifndef HOST
  ifeq ($(OS),Windows_NT)
    HOST = windows
  else
    uname_s := $(shell uname -s)
    ifeq ($(uname_s),Darwin)
      HOST = macos
    else ifeq ($(uname_s),Linux)
      HOST = linux
    else
      $(error Your OS wasn't recognized, please manually define HOST. For instance, 'make HOST=windows' 'make HOST=linux' 'make HOST=macos')
    endif
  endif
endif

# Providing PLATFORM=simulator will automatically select the host platform.
ifneq ($(filter simulator host,$(PLATFORM)),)
  override PLATFORM := $(HOST)
endif

ifeq ($(PLATFORM),device)
  PLATFORM_CATEGORY = device
else
  PLATFORM_CATEGORY = simulator
endif

SIMULATORS_DIR = ../epsilon_simulators

ifeq ($(PLATFORM),device)
  CC = arm-none-eabi-gcc
  CXX = arm-none-eabi-g++
  LINK_GC = 1
  LTO = 1
else ifeq ($(PLATFORM),web)
  CC = emcc
  CXX = em++
  LINK_GC = 0
  LTO = 0
  SIMULATOR ?= $(SIMULATORS_DIR)/$(PLATFORM)/epsilon.html
else
  SIMULATOR_PATH =
  LD_DYNAMIC_LOOKUP_FLAG = -Wl,-undefined,dynamic_lookup
  ifeq ($(HOST),windows)
    ifeq ($(OS),Windows_NT)
      MINGW_TOOLCHAIN_PREFIX=
    else
      MINGW_TOOLCHAIN_PREFIX=x86_64-w64-mingw32-
    endif
    CC = $(MINGW_TOOLCHAIN_PREFIX)gcc
    CXX = $(MINGW_TOOLCHAIN_PREFIX)g++
    GDB = $(MINGW_TOOLCHAIN_PREFIX)gdb --args
    EXE = exe
    LD_DYNAMIC_LOOKUP_FLAG = -L$(SIMULATORS_DIR)/$(PLATFORM) -lepsilon
  else ifeq ($(HOST),linux)
    CC = gcc
    CXX = g++
    GDB = gdb --args
    EXE = bin
  else
    CXX = clang++
    GDB = lldb --
    EXE = app
    SIMULATOR_PATH = /Contents/MacOS/Epsilon
  endif
  LINK_GC = 0
  LTO = 0
  SIMULATOR ?= $(SIMULATORS_DIR)/$(PLATFORM)/epsilon.$(EXE)$(SIMULATOR_PATH)
endif

NWLINK = npx --yes -- nwlink
BUILD_DIR = $(OUTPUT_DIR)/$(PLATFORM)

define object_for
  $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef


CFLAGS = $(shell $(NWLINK) eadk-cflags-$(PLATFORM_CATEGORY))
ifeq ($(PLATFORM),web)
  # TODO: Update nwlink's eadk-ldflags-web : eadk_keyboard_scan_do_scan is actually _eadk_keyboard_scan_do_scan
  LDFLAGS = -sSIDE_MODULE=2 -sEXPORTED_FUNCTIONS=_main -sASYNCIFY=1 -sASYNCIFY_IMPORTS=eadk_event_get,_eadk_keyboard_scan_do_scan,eadk_timing_msleep,eadk_display_wait_for_vblank
else
  LDFLAGS = $(shell $(NWLINK) eadk-ldflags-$(PLATFORM_CATEGORY))
endif

CXXFLAGS = $(CFLAGS) -std=c++11 -fno-exceptions -Wno-nullability-completeness -Wall -ggdb

ifeq ($(PLATFORM),device)
  CXXFLAGS += -Os
  LDFLAGS += --specs=nano.specs
  # LDFLAGS += --specs=nosys.specs # Alternatively, use full-fledged newlib
else
  CXXFLAGS += -O0 -g
  ifeq ($(PLATFORM),web)
    LDFLAGS += -lc
  else
    LDFLAGS += $(LD_DYNAMIC_LOOKUP_FLAG)
  endif
endif

ifeq ($(LINK_GC),1)
  CXXFLAGS += -fdata-sections -ffunction-sections
  LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
  LDFLAGS += -Wl,--gc-sections
endif

ifeq ($(LTO),1)
  CXXFLAGS += -flto -fno-fat-lto-objects
  CXXFLAGS += -fwhole-program
  CXXFLAGS += -fvisibility=internal
  LDFLAGS += -flinker-output=nolto-rel
endif

# External data
ifdef EXTERNAL_DATA
  ifeq ($(PLATFORM),device)
    EXTERNAL_DATA_INPUT = --external-data $(EXTERNAL_DATA)
  else ifeq ($(PLATFORM),web)
    EXTERNAL_DATA_INPUT = "&nwbdata=/$(EXTERNAL_DATA)"
  else
    EXTERNAL_DATA_INPUT = --nwb-external-data $(EXTERNAL_DATA)
  endif
else
  EXTERNAL_DATA =
  EXTERNAL_DATA_INPUT =
endif

ifeq ($(PLATFORM),device)

.PHONY: build
build: $(BUILD_DIR)/$(APP_NAME).nwa

.PHONY: run
run: $(BUILD_DIR)/$(APP_NAME).nwa $(EXTERNAL_DATA)
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa $(EXTERNAL_DATA_INPUT) $<

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.nwa $(EXTERNAL_DATA)
	@echo "BIN     $@"
	$(Q) $(NWLINK) nwa-bin $(EXTERNAL_DATA_INPUT) $< $@

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.nwa $(EXTERNAL_DATA)
	@echo "ELF     $@"
	$(Q) $(NWLINK) nwa-elf $(EXTERNAL_DATA_INPUT) $< $@

$(BUILD_DIR)/$(APP_NAME).nwa: $(call object_for,$(SOURCES)) $(BUILD_DIR)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

else

.PHONY: build
build: $(BUILD_DIR)/$(APP_NAME).nwb

$(BUILD_DIR)/$(APP_NAME).nwb: $(call object_for,$(SOURCES)) $(SIMULATOR)
	@echo "LD      $@"
	$(Q) $(CC) $(CXXFLAGS) $(call object_for,$(SOURCES)) $(LDFLAGS) -o $@

  ifeq ($(PLATFORM),web)
.PHONY: server
server: $(SIMULATOR) $(EXTERNAL_DATA)
	@echo "STARTING SERVER"
	$(Q) python3 -m http.server

# Simulator is copied because python http server cannot access files outside of the current directory.
.PHONY: run
run: $(BUILD_DIR)/$(APP_NAME).nwb $(SIMULATOR) $(EXTERNAL_DATA)
    ifeq ($(OS),Windows_NT)
	copy $(SIMULATOR) $(BUILD_DIR)/epsilon.html
    else
	cp $(SIMULATOR) $(BUILD_DIR)/epsilon.html
    endif
	@echo "RUN     $<"
    ifeq ($(OS),Windows_NT)
	$(Q) powershell -Command "Start-Process http://localhost:8000/$(BUILD_DIR)/epsilon.html?nwb=/$<$(EXTERNAL_DATA_INPUT)"
    else ifeq ($(HOST),linux)
	$(Q) xdg-open http://localhost:8000/$(BUILD_DIR)/epsilon.html?nwb=/$<$(EXTERNAL_DATA_INPUT)
    else
	$(Q) open http://localhost:8000/$(BUILD_DIR)/epsilon.html?nwb=/$<$(EXTERNAL_DATA_INPUT)
    endif
  else
.PHONY: run
run: $(BUILD_DIR)/$(APP_NAME).nwb $(SIMULATOR) $(EXTERNAL_DATA)
	@echo "RUN     $<"
	$(Q) $(SIMULATOR) --nwb $< $(EXTERNAL_DATA_INPUT)

.PHONY: debug
debug: $(BUILD_DIR)/$(APP_NAME).nwb $(SIMULATOR) $(EXTERNAL_DATA)
	@echo "DEBUG   $<"
	$(Q) $(GDB) $(SIMULATOR) --nwb $< $(EXTERNAL_DATA_INPUT)

  endif
endif

$(addprefix $(BUILD_DIR)/,%.o): %.cpp | $(BUILD_DIR)
	@echo "CC      $^"
	$(Q) $(CXX) $(CXXFLAGS) -c $^ -o $@

$(addprefix $(BUILD_DIR)/,%.o): %.c | $(BUILD_DIR)
	@echo "CC      $^"
	$(Q) $(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/icon.o: $(APP_ICON)
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PRECIOUS: $(BUILD_DIR)
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	$(Q) powershell -Command "mkdir $@/src"
else
	mkdir -p $@/src
endif

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR)
