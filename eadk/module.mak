ifeq ($(PLATFORM_TYPE),simulator)

$(call create_module,eadk,1, \
  src/simulator.cpp \
)

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_eadk)): $(KANDINSKY_fonts_dependencies)

else

$(call create_module,eadk,1,$(addprefix src/, \
  platform.c \
  svc.cpp \
  trampoline.cpp \
))

SFLAGS_eadk += \
  -fno-lto \
  -I$(PATH_eadk)/../liba/include \
  -I$(PATH_eadk)/../ion/src/device

LDFLAGS_eadk += -Wl,--relocatable # Make a single .o file
LDFLAGS_eadk += -flinker-output=nolto-rel # Don't output an LTO object
LDFLAGS_eadk += -Wl,--no-gc-sections

$(call create_goal,eadk,eadk)

.PHONY: eadk.o
eadk.o: $(OUTPUT_DIRECTORY)/eadk.o
$(OUTPUT_DIRECTORY)/eadk.o: $(OUTPUT_DIRECTORY)/eadk.elf
	$(call rule_label,STRIP)
	$(Q) arm-none-eabi-strip --strip-debug --discard-all $< -o $@

endif
