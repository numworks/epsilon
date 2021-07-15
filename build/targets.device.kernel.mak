kernel_src = $(ion_device_kernel_src) $(liba_kernel_src) $(kandinsky_minimal_src)
kernel_obj = $(call flavored_object_for,$(kernel_src),)

KERNEL_LDFLAGS = -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/$(MODEL)/kernel
KERNEL_LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/kernel/$(subst .,_,$*)_flash.ld
