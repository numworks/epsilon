
USERMODULES_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(USERMODULES_DIR)/scipy/linalg/linalg.c
SRC_USERMOD += $(USERMODULES_DIR)/scipy/optimize/optimize.c
SRC_USERMOD += $(USERMODULES_DIR)/scipy/signal/signal.c
SRC_USERMOD += $(USERMODULES_DIR)/scipy/special/special.c
SRC_USERMOD += $(USERMODULES_DIR)/ndarray_operators.c
SRC_USERMOD += $(USERMODULES_DIR)/ulab_tools.c
SRC_USERMOD += $(USERMODULES_DIR)/ndarray.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/ndarray/ndarray_iter.c
SRC_USERMOD += $(USERMODULES_DIR)/ndarray_properties.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/approx.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/compare.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/carray/carray.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/carray/carray_tools.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/create.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/fft/fft.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/fft/fft_tools.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/filter.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/io/io.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/linalg/linalg.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/linalg/linalg_tools.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/numerical.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/poly.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/stats.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/transform.c
SRC_USERMOD += $(USERMODULES_DIR)/numpy/vector.c

SRC_USERMOD += $(USERMODULES_DIR)/numpy/numpy.c
SRC_USERMOD += $(USERMODULES_DIR)/scipy/scipy.c
SRC_USERMOD += $(USERMODULES_DIR)/user/user.c
SRC_USERMOD += $(USERMODULES_DIR)/utils/utils.c
SRC_USERMOD += $(USERMODULES_DIR)/ulab.c

CFLAGS_USERMOD += -I$(USERMODULES_DIR)

override CFLAGS_EXTRA += -DMODULE_ULAB_ENABLED=1
