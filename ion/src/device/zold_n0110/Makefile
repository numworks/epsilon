include ion/src/device/n0110/bootloader_permanent/Makefile
include ion/src/device/n0110/bootloader_updatable/Makefile
include ion/src/device/n0110/external_flash/Makefile

ion_device_src += $(addprefix ion/src/device/n0110/drivers/, \
  board_privileged.cpp:+privileged \
  board.cpp\
  cache.cpp:+privileged \
  flash.cpp \
  led.cpp \
  power_unprivileged.cpp:-privileged \
  power_privileged.cpp:+privileged \
  reset.cpp \
)

LDFLAGS += -Lion/src/device/n0110
LDSCRIPT ?= ion/src/device/n0110/external_flash.ld
