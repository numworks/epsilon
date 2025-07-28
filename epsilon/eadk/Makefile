eadk_src = $(addprefix eadk/src/, \
  platform.c \
  svc.cpp \
  trampoline.c \
)

EADK_LDFLAGS = $(SFLAGS)
EADK_LDFLAGS += -Wl,--relocatable # Make a single .o file
EADK_LDFLAGS += -flinker-output=nolto-rel # Don't output an LTO object

$(call object_for,$(eadk_src)): SFLAGS := $(filter-out -flto,$(SFLAGS))
$(call object_for,$(eadk_src)): SFLAGS += -Iion/src/device #Just to make include in userland/svcall.h happy. Maybe not the best way.

.PHONY: eadk.o
eadk.o: $(BUILD_DIR)/eadk.o
$(BUILD_DIR)/eadk.o: $(call object_for,$(eadk_src))
	$(call rule_label,LD)
	$(Q) $(LD) $(EADK_LDFLAGS) $^ -o $@
	$(call rule_label,STRIP)
	$(Q) arm-none-eabi-strip --strip-debug --discard-all $@
