kernel_src = $(ion_device_kernel_src) $(liba_kernel_src) $(kandinsky_minimal_src)

KERNEL_LDFLAGS = -Lion/src/$(PLATFORM)/$(MODEL)/kernel -Lion/src/$(PLATFORM)/kernel
KERNEL_LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/kernel/$(subst .,_,$*)_flash.ld
KERNEL_LDDEPS += $(KERNEL_LDSCRIPT) ion/src/$(PLATFORM)/kernel/shared_kernel_flash.ld

