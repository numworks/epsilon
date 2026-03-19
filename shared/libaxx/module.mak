$(call create_module,libaxx,1, \
  src/cxxabi/atexit.cpp \
  src/cxxabi/pure_virtual.cpp \
)

ifeq ($(LIBAXX_real_complexes),1)
# In libcpp innocuous headers such as array can pull an incompatible
# forward declaration of std::complex. We define its header guard to
# prevent it from interfering.
SFLAGS += \
  -D_LIBCPP___FWD_COMPLEX_H \
  -I$(PATH_libaxx)/include/real_complex_wrapper
endif
