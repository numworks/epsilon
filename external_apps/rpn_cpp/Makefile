# Configuration
Q ?= @
PLATFORM ?= web

SOURCES = $(addprefix src/,\
  converter.cpp \
  input_field.cpp \
  main.cpp \
  store.cpp \
  view.cpp \
)

# Verbose
ifeq ("$(origin V)", "command line")
  ifeq ($(V),1)
    Q=
  endif
endif

# Host detection
ifeq ($(PLATFORM),simulator)
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

ifeq ($(PLATFORM),device)
  CC = arm-none-eabi-gcc
  CXX = arm-none-eabi-g++
  LINK_GC = 1
  LTO = 1
else ifeq ($(PLATFORM),simulator)
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
    LD_DYNAMIC_LOOKUP_FLAG = -Lepsilon_simulators/$(HOST) -lepsilon
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
  SIMULATOR ?= epsilon_simulators/$(HOST)/epsilon.$(EXE)$(SIMULATOR_PATH)
else # PLATFORM=web
  CC = emcc
  CXX = em++
  LINK_GC = 0
  LTO = 0
  SIMULATOR ?= epsilon_simulators/web/epsilon.html
endif

NWLINK = npx --yes -- nwlink@0.0.19
BUILD_DIR = output/$(PLATFORM)

define object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef


CFLAGS = $(shell $(NWLINK) eadk-cflags-$(PLATFORM))
LDFLAGS = $(shell $(NWLINK) eadk-ldflags-$(PLATFORM))
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-exceptions -Wno-nullability-completeness -Wall -ggdb

ifeq ($(PLATFORM),device)
CXXFLAGS += -Os
LDFLAGS += --specs=nano.specs
# LDFLAGS += --specs=nosys.specs # Alternatively, use full-fledged newlib
else ifeq ($(PLATFORM),simulator)
CXXFLAGS += -O0 -g
LDFLAGS += $(LD_DYNAMIC_LOOKUP_FLAG)
else # PLATFORM=web
CXXFLAGS += -O0 -g
LDFLAGS += -lc
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

ifeq ($(PLATFORM),device)

.PHONY: build
build: $(BUILD_DIR)/app.nwa

.PHONY: check
check: $(BUILD_DIR)/app.bin

.PHONY: run
run: $(BUILD_DIR)/app.nwa
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa $<

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.nwa
	@echo "BIN     $@"
	$(Q) $(NWLINK) nwa-bin $< $@

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.nwa
	@echo "ELF     $@"
	$(Q) $(NWLINK) nwa-elf $< $@

$(BUILD_DIR)/app.nwa: $(call object_for,$(SOURCES)) $(BUILD_DIR)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

else

.PHONY: build
build: $(BUILD_DIR)/app.nwb

$(BUILD_DIR)/app.nwb: $(call object_for,$(SOURCES)) $(SIMULATOR)
	@echo "LD      $@"
	$(Q) $(CC) $(CXXFLAGS) $(call object_for,$(SOURCES)) $(LDFLAGS) -o $@

ifeq ($(PLATFORM),simulator)

$(SIMULATOR):
	@echo "UNZIP   $<"
ifeq ($(OS),Windows_NT)
	$(Q) powershell -Command "Expand-Archive -Path epsilon_simulators.zip -DestinationPath ."
else
	$(Q) unzip epsilon_simulators.zip
	$(Q) chmod +x $(SIMULATOR)
endif

.PHONY: run
run: $(BUILD_DIR)/app.nwb $(SIMULATOR)
	@echo "RUN     $<"
	$(Q) $(SIMULATOR) --nwb $<

.PHONY: debug
debug: $(BUILD_DIR)/app.nwb $(SIMULATOR)
	@echo "DEBUG   $<"
	$(Q) $(GDB) $(SIMULATOR) --nwb $<

else # PLATFORM=web

$(SIMULATOR):
	@echo "UNZIP   $<"
ifeq ($(OS),Windows_NT)
	$(Q) powershell -Command "Expand-Archive -Path web_simulator.zip -DestinationPath ."
else
	$(Q) unzip web_simulator.zip
endif

.PHONY: server
server: $(SIMULATOR)
	@echo "STARTING SERVER"
	$(Q) python3 -m http.server

.PHONY: run
run: $(BUILD_DIR)/app.nwb $(SIMULATOR)
	@echo "RUN     $<"
ifeq ($(OS),Windows_NT)
	$(Q) powershell -Command "Start-Process http://localhost:8000/$(SIMULATOR)?nwb=/$<"
else ifeq ($(HOST),linux)
	$(Q) xdg-open http://localhost:8000/$(SIMULATOR)?nwb=/$<
else
	$(Q) open http://localhost:8000/$(SIMULATOR)?nwb=/$<
endif

endif
endif

$(addprefix $(BUILD_DIR)/,%.o): %.cpp | $(BUILD_DIR)
	@echo "CC      $^"
	$(Q) $(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILD_DIR)/icon.o: src/icon.png
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
