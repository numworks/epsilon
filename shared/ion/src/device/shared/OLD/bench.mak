ion_device_bench_src += ion/src/device/shared/boot/rt0.cpp

ion_device_bench_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  serial_number.cpp \
  trampoline.cpp \
)

ion_device_bench_src += ion/src/device/shared/post_and_hardware_tests
