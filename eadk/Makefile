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

eadk.o: $(call object_for,$(eadk_src))
	@echo "LD      $@"
	$(Q) $(LD) $(EADK_LDFLAGS) $^ -o $@
