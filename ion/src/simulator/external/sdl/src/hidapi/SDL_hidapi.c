/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* Original hybrid wrapper for Linux by Valve Software. Their original notes:
 *
 * The libusb version doesn't support Bluetooth, but not all Linux
 * distributions allow access to /dev/hidraw*
 *
 * This merges the two, at a small performance cost, until distributions
 * have granted access to /dev/hidraw*
 */

#include "../SDL_internal.h"
#include "SDL_loadso.h"

#ifdef SDL_JOYSTICK_HIDAPI

/* Platform HIDAPI Implementation */

#define hid_device_                     PLATFORM_hid_device_
#define hid_device                      PLATFORM_hid_device
#define hid_device_info                 PLATFORM_hid_device_info
#define hid_init                        PLATFORM_hid_init
#define hid_exit                        PLATFORM_hid_exit
#define hid_enumerate                   PLATFORM_hid_enumerate
#define hid_free_enumeration            PLATFORM_hid_free_enumeration
#define hid_open                        PLATFORM_hid_open
#define hid_open_path                   PLATFORM_hid_open_path
#define hid_write                       PLATFORM_hid_write
#define hid_read_timeout                PLATFORM_hid_read_timeout
#define hid_read                        PLATFORM_hid_read
#define hid_set_nonblocking             PLATFORM_hid_set_nonblocking
#define hid_send_feature_report         PLATFORM_hid_send_feature_report
#define hid_get_feature_report          PLATFORM_hid_get_feature_report
#define hid_close                       PLATFORM_hid_close
#define hid_get_manufacturer_string     PLATFORM_hid_get_manufacturer_string
#define hid_get_product_string          PLATFORM_hid_get_product_string
#define hid_get_serial_number_string    PLATFORM_hid_get_serial_number_string
#define hid_get_indexed_string          PLATFORM_hid_get_indexed_string
#define hid_error                       PLATFORM_hid_error
#define new_hid_device                  PLATFORM_new_hid_device
#define free_hid_device                 PLATFORM_free_hid_device
#define input_report                    PLATFORM_input_report
#define return_data                     PLATFORM_return_data
#define make_path                       PLATFORM_make_path
#define read_thread                     PLATFORM_read_thread

#if __LINUX__

#include "../../core/linux/SDL_udev.h"
#if SDL_USE_LIBUDEV
static const SDL_UDEV_Symbols *udev_ctx = NULL;

#define udev_device_get_sysattr_value                    udev_ctx->udev_device_get_sysattr_value
#define udev_new                                         udev_ctx->udev_new
#define udev_unref                                       udev_ctx->udev_unref
#define udev_device_new_from_devnum                      udev_ctx->udev_device_new_from_devnum
#define udev_device_get_parent_with_subsystem_devtype    udev_ctx->udev_device_get_parent_with_subsystem_devtype
#define udev_device_unref                                udev_ctx->udev_device_unref
#define udev_enumerate_new                               udev_ctx->udev_enumerate_new
#define udev_enumerate_add_match_subsystem               udev_ctx->udev_enumerate_add_match_subsystem
#define udev_enumerate_scan_devices                      udev_ctx->udev_enumerate_scan_devices
#define udev_enumerate_get_list_entry                    udev_ctx->udev_enumerate_get_list_entry
#define udev_list_entry_get_name                         udev_ctx->udev_list_entry_get_name
#define udev_device_new_from_syspath                     udev_ctx->udev_device_new_from_syspath
#define udev_device_get_devnode                          udev_ctx->udev_device_get_devnode
#define udev_list_entry_get_next                         udev_ctx->udev_list_entry_get_next
#define udev_enumerate_unref                             udev_ctx->udev_enumerate_unref

#include "linux/hid.c"
#define HAVE_PLATFORM_BACKEND 1
#endif /* SDL_USE_LIBUDEV */

#elif __MACOSX__
#include "mac/hid.c"
#define HAVE_PLATFORM_BACKEND 1
#define udev_ctx 1
#elif __WINDOWS__
#include "windows/hid.c"
#define HAVE_PLATFORM_BACKEND 1
#define udev_ctx 1
#else
#error Need a hid.c for this platform!
#endif

#undef hid_device_
#undef hid_device
#undef hid_device_info
#undef hid_init
#undef hid_exit
#undef hid_enumerate
#undef hid_free_enumeration
#undef hid_open
#undef hid_open_path
#undef hid_write
#undef hid_read_timeout
#undef hid_read
#undef hid_set_nonblocking
#undef hid_send_feature_report
#undef hid_get_feature_report
#undef hid_close
#undef hid_get_manufacturer_string
#undef hid_get_product_string
#undef hid_get_serial_number_string
#undef hid_get_indexed_string
#undef hid_error
#undef new_hid_device
#undef free_hid_device
#undef input_report
#undef return_data
#undef make_path
#undef read_thread

#ifndef SDL_LIBUSB_DYNAMIC
#if __WINDOWS__
#define SDL_LIBUSB_DYNAMIC "libusb-1.0.dll"
#endif /* __WINDOWS__ */
#endif /* SDL_LIBUSB_DYNAMIC */

#ifdef SDL_LIBUSB_DYNAMIC
/* libusb HIDAPI Implementation */

/* Include this now, for our dynamically-loaded libusb context */
#include <libusb.h>

static struct
{
    void* libhandle;

    int (*init)(libusb_context **ctx);
    void (*exit)(libusb_context *ctx);
    ssize_t (*get_device_list)(libusb_context *ctx, libusb_device ***list);
    void (*free_device_list)(libusb_device **list, int unref_devices);
    int (*get_device_descriptor)(libusb_device *dev, struct libusb_device_descriptor *desc);
    int (*get_active_config_descriptor)(libusb_device *dev,    struct libusb_config_descriptor **config);
    int (*get_config_descriptor)(
        libusb_device *dev,
        uint8_t config_index,
        struct libusb_config_descriptor **config
    );
    void (*free_config_descriptor)(struct libusb_config_descriptor *config);
    uint8_t (*get_bus_number)(libusb_device *dev);
    uint8_t (*get_device_address)(libusb_device *dev);
    int (*open)(libusb_device *dev, libusb_device_handle **dev_handle);
    void (*close)(libusb_device_handle *dev_handle);
    int (*claim_interface)(libusb_device_handle *dev_handle, int interface_number);
    int (*release_interface)(libusb_device_handle *dev_handle, int interface_number);
    int (*kernel_driver_active)(libusb_device_handle *dev_handle, int interface_number);
    int (*detach_kernel_driver)(libusb_device_handle *dev_handle, int interface_number);
    struct libusb_transfer * (*alloc_transfer)(int iso_packets);
    int (*submit_transfer)(struct libusb_transfer *transfer);
    int (*cancel_transfer)(struct libusb_transfer *transfer);
    void (*free_transfer)(struct libusb_transfer *transfer);
    int (*control_transfer)(
        libusb_device_handle *dev_handle,
        uint8_t request_type,
        uint8_t bRequest,
        uint16_t wValue,
        uint16_t wIndex,
        unsigned char *data,
        uint16_t wLength,
        unsigned int timeout
    );
    int (*interrupt_transfer)(
        libusb_device_handle *dev_handle,
        unsigned char endpoint,
        unsigned char *data,
        int length,
        int *actual_length,
        unsigned int timeout
    );
    int (*handle_events)(libusb_context *ctx);
    int (*handle_events_completed)(libusb_context *ctx, int *completed);
} libusb_ctx;

#define libusb_init                            libusb_ctx.init
#define libusb_exit                            libusb_ctx.exit
#define libusb_get_device_list                 libusb_ctx.get_device_list
#define libusb_free_device_list                libusb_ctx.free_device_list
#define libusb_get_device_descriptor           libusb_ctx.get_device_descriptor
#define libusb_get_active_config_descriptor    libusb_ctx.get_active_config_descriptor
#define libusb_get_config_descriptor           libusb_ctx.get_config_descriptor
#define libusb_free_config_descriptor          libusb_ctx.free_config_descriptor
#define libusb_get_bus_number                  libusb_ctx.get_bus_number
#define libusb_get_device_address              libusb_ctx.get_device_address
#define libusb_open                            libusb_ctx.open
#define libusb_close                           libusb_ctx.close
#define libusb_claim_interface                 libusb_ctx.claim_interface
#define libusb_release_interface               libusb_ctx.release_interface
#define libusb_kernel_driver_active            libusb_ctx.kernel_driver_active
#define libusb_detach_kernel_driver            libusb_ctx.detach_kernel_driver
#define libusb_alloc_transfer                  libusb_ctx.alloc_transfer
#define libusb_submit_transfer                 libusb_ctx.submit_transfer
#define libusb_cancel_transfer                 libusb_ctx.cancel_transfer
#define libusb_free_transfer                   libusb_ctx.free_transfer
#define libusb_control_transfer                libusb_ctx.control_transfer
#define libusb_interrupt_transfer              libusb_ctx.interrupt_transfer
#define libusb_handle_events                   libusb_ctx.handle_events
#define libusb_handle_events_completed         libusb_ctx.handle_events_completed

#define hid_device_                     LIBUSB_hid_device_
#define hid_device                      LIBUSB_hid_device
#define hid_device_info                 LIBUSB_hid_device_info
#define hid_init                        LIBUSB_hid_init
#define hid_exit                        LIBUSB_hid_exit
#define hid_enumerate                   LIBUSB_hid_enumerate
#define hid_free_enumeration            LIBUSB_hid_free_enumeration
#define hid_open                        LIBUSB_hid_open
#define hid_open_path                   LIBUSB_hid_open_path
#define hid_write                       LIBUSB_hid_write
#define hid_read_timeout                LIBUSB_hid_read_timeout
#define hid_read                        LIBUSB_hid_read
#define hid_set_nonblocking             LIBUSB_hid_set_nonblocking
#define hid_send_feature_report         LIBUSB_hid_send_feature_report
#define hid_get_feature_report          LIBUSB_hid_get_feature_report
#define hid_close                       LIBUSB_hid_close
#define hid_get_manufacturer_string     LIBUSB_hid_get_manufacturer_string
#define hid_get_product_string          LIBUSB_hid_get_product_string
#define hid_get_serial_number_string    LIBUSB_hid_get_serial_number_string
#define hid_get_indexed_string          LIBUSB_hid_get_indexed_string
#define hid_error                       LIBUSB_hid_error
#define new_hid_device                  LIBUSB_new_hid_device
#define free_hid_device                 LIBUSB_free_hid_device
#define input_report                    LIBUSB_input_report
#define return_data                     LIBUSB_return_data
#define make_path                       LIBUSB_make_path
#define read_thread                     LIBUSB_read_thread

#ifndef __FreeBSD__
/* this is awkwardly inlined, so we need to re-implement it here
 * so we can override the libusb_control_transfer call */
static int
SDL_libusb_get_string_descriptor(libusb_device_handle *dev,
                                 uint8_t descriptor_index, uint16_t lang_id,
                                 unsigned char *data, int length)
{
    return libusb_control_transfer(dev,
                                   LIBUSB_ENDPOINT_IN | 0x0, /* Endpoint 0 IN */
                                   LIBUSB_REQUEST_GET_DESCRIPTOR,
                                   (LIBUSB_DT_STRING << 8) | descriptor_index,
                                   lang_id,
                                   data,
                                   (uint16_t) length,
                                   1000);
}
#define libusb_get_string_descriptor SDL_libusb_get_string_descriptor
#endif /* __FreeBSD__ */

#undef HIDAPI_H__
#include "libusb/hid.c"

#undef hid_device_
#undef hid_device
#undef hid_device_info
#undef hid_init
#undef hid_exit
#undef hid_enumerate
#undef hid_free_enumeration
#undef hid_open
#undef hid_open_path
#undef hid_write
#undef hid_read_timeout
#undef hid_read
#undef hid_set_nonblocking
#undef hid_send_feature_report
#undef hid_get_feature_report
#undef hid_close
#undef hid_get_manufacturer_string
#undef hid_get_product_string
#undef hid_get_serial_number_string
#undef hid_get_indexed_string
#undef hid_error
#undef new_hid_device
#undef free_hid_device
#undef input_report
#undef return_data
#undef make_path
#undef read_thread

#endif /* SDL_LIBUSB_DYNAMIC */

/* Shared HIDAPI Implementation */

#undef HIDAPI_H__
#include "hidapi.h"

struct hidapi_backend {
#define F(x) typeof(x) *x
    F(hid_write);
    F(hid_read_timeout);
    F(hid_read);
    F(hid_set_nonblocking);
    F(hid_send_feature_report);
    F(hid_get_feature_report);
    F(hid_close);
    F(hid_get_manufacturer_string);
    F(hid_get_product_string);
    F(hid_get_serial_number_string);
    F(hid_get_indexed_string);
    F(hid_error);
#undef F
};

#if HAVE_PLATFORM_BACKEND
static const struct hidapi_backend PLATFORM_Backend = {
    (void*)PLATFORM_hid_write,
    (void*)PLATFORM_hid_read_timeout,
    (void*)PLATFORM_hid_read,
    (void*)PLATFORM_hid_set_nonblocking,
    (void*)PLATFORM_hid_send_feature_report,
    (void*)PLATFORM_hid_get_feature_report,
    (void*)PLATFORM_hid_close,
    (void*)PLATFORM_hid_get_manufacturer_string,
    (void*)PLATFORM_hid_get_product_string,
    (void*)PLATFORM_hid_get_serial_number_string,
    (void*)PLATFORM_hid_get_indexed_string,
    (void*)PLATFORM_hid_error
};
#endif /* HAVE_PLATFORM_BACKEND */

#ifdef SDL_LIBUSB_DYNAMIC
static const struct hidapi_backend LIBUSB_Backend = {
    (void*)LIBUSB_hid_write,
    (void*)LIBUSB_hid_read_timeout,
    (void*)LIBUSB_hid_read,
    (void*)LIBUSB_hid_set_nonblocking,
    (void*)LIBUSB_hid_send_feature_report,
    (void*)LIBUSB_hid_get_feature_report,
    (void*)LIBUSB_hid_close,
    (void*)LIBUSB_hid_get_manufacturer_string,
    (void*)LIBUSB_hid_get_product_string,
    (void*)LIBUSB_hid_get_serial_number_string,
    (void*)LIBUSB_hid_get_indexed_string,
    (void*)LIBUSB_hid_error
};
#endif /* SDL_LIBUSB_DYNAMIC */

typedef struct _HIDDeviceWrapper HIDDeviceWrapper;
struct _HIDDeviceWrapper
{
    hid_device *device; /* must be first field */
    const struct hidapi_backend *backend;
};

static HIDDeviceWrapper *
CreateHIDDeviceWrapper(hid_device *device, const struct hidapi_backend *backend)
{
    HIDDeviceWrapper *ret = SDL_malloc(sizeof(*ret));
    ret->device = device;
    ret->backend = backend;
    return ret;
}

static hid_device *
WrapHIDDevice(HIDDeviceWrapper *wrapper)
{
    return (hid_device *)wrapper;
}

static HIDDeviceWrapper *
UnwrapHIDDevice(hid_device *device)
{
    return (HIDDeviceWrapper *)device;
}

static void
DeleteHIDDeviceWrapper(HIDDeviceWrapper *device)
{
    SDL_free(device);
}

#define COPY_IF_EXISTS(var) \
    if (pSrc->var != NULL) { \
        pDst->var = SDL_strdup(pSrc->var); \
    } else { \
        pDst->var = NULL; \
    }
#define WCOPY_IF_EXISTS(var) \
    if (pSrc->var != NULL) { \
        pDst->var = SDL_wcsdup(pSrc->var); \
    } else { \
        pDst->var = NULL; \
    }

#ifdef SDL_LIBUSB_DYNAMIC
static void
LIBUSB_CopyHIDDeviceInfo(struct LIBUSB_hid_device_info *pSrc,
                         struct hid_device_info *pDst)
{
    COPY_IF_EXISTS(path)
    pDst->vendor_id = pSrc->vendor_id;
    pDst->product_id = pSrc->product_id;
    WCOPY_IF_EXISTS(serial_number)
    pDst->release_number = pSrc->release_number;
    WCOPY_IF_EXISTS(manufacturer_string)
    WCOPY_IF_EXISTS(product_string)
    pDst->usage_page = pSrc->usage_page;
    pDst->usage = pSrc->usage;
    pDst->interface_number = pSrc->interface_number;
    pDst->next = NULL;
}
#endif /* SDL_LIBUSB_DYNAMIC */

#if HAVE_PLATFORM_BACKEND
static void
PLATFORM_CopyHIDDeviceInfo(struct PLATFORM_hid_device_info *pSrc,
                           struct hid_device_info *pDst)
{
    COPY_IF_EXISTS(path)
    pDst->vendor_id = pSrc->vendor_id;
    pDst->product_id = pSrc->product_id;
    WCOPY_IF_EXISTS(serial_number)
    pDst->release_number = pSrc->release_number;
    WCOPY_IF_EXISTS(manufacturer_string)
    WCOPY_IF_EXISTS(product_string)
    pDst->usage_page = pSrc->usage_page;
    pDst->usage = pSrc->usage;
    pDst->interface_number = pSrc->interface_number;
    pDst->next = NULL;
}
#endif /* HAVE_PLATFORM_BACKEND */

#undef COPY_IF_EXISTS
#undef WCOPY_IF_EXISTS

static SDL_bool SDL_hidapi_wasinit = SDL_FALSE;

int HID_API_EXPORT HID_API_CALL hid_init(void)
{
    int err;

    if (SDL_hidapi_wasinit == SDL_TRUE) {
        return 0;
    }

#ifdef SDL_LIBUSB_DYNAMIC
    libusb_ctx.libhandle = SDL_LoadObject(SDL_LIBUSB_DYNAMIC);
    if (libusb_ctx.libhandle != NULL) {
        #define LOAD_LIBUSB_SYMBOL(func) \
            libusb_ctx.func = SDL_LoadFunction(libusb_ctx.libhandle, "libusb_" #func);
        LOAD_LIBUSB_SYMBOL(init)
        LOAD_LIBUSB_SYMBOL(exit)
        LOAD_LIBUSB_SYMBOL(get_device_list)
        LOAD_LIBUSB_SYMBOL(free_device_list)
        LOAD_LIBUSB_SYMBOL(get_device_descriptor)
        LOAD_LIBUSB_SYMBOL(get_active_config_descriptor)
        LOAD_LIBUSB_SYMBOL(get_config_descriptor)
        LOAD_LIBUSB_SYMBOL(free_config_descriptor)
        LOAD_LIBUSB_SYMBOL(get_bus_number)
        LOAD_LIBUSB_SYMBOL(get_device_address)
        LOAD_LIBUSB_SYMBOL(open)
        LOAD_LIBUSB_SYMBOL(close)
        LOAD_LIBUSB_SYMBOL(claim_interface)
        LOAD_LIBUSB_SYMBOL(release_interface)
        LOAD_LIBUSB_SYMBOL(kernel_driver_active)
        LOAD_LIBUSB_SYMBOL(detach_kernel_driver)
        LOAD_LIBUSB_SYMBOL(alloc_transfer)
        LOAD_LIBUSB_SYMBOL(submit_transfer)
        LOAD_LIBUSB_SYMBOL(cancel_transfer)
        LOAD_LIBUSB_SYMBOL(free_transfer)
        LOAD_LIBUSB_SYMBOL(control_transfer)
        LOAD_LIBUSB_SYMBOL(interrupt_transfer)
        LOAD_LIBUSB_SYMBOL(handle_events)
        LOAD_LIBUSB_SYMBOL(handle_events_completed)
        #undef LOAD_LIBUSB_SYMBOL

        if ((err = LIBUSB_hid_init()) < 0) {
            SDL_UnloadObject(libusb_ctx.libhandle);
            return err;
        }
    }
#endif /* SDL_LIBUSB_DYNAMIC */

#if HAVE_PLATFORM_BACKEND
#if __LINUX__
    udev_ctx = SDL_UDEV_GetUdevSyms();
#endif /* __LINUX __ */
    if (udev_ctx && (err = PLATFORM_hid_init()) < 0) {
#ifdef SDL_LIBUSB_DYNAMIC
        if (libusb_ctx.libhandle) {
            SDL_UnloadObject(libusb_ctx.libhandle);
        }
#endif /* SDL_LIBUSB_DYNAMIC */
        return err;
    }
#endif /* HAVE_PLATFORM_BACKEND */

    return 0;
}

int HID_API_EXPORT HID_API_CALL hid_exit(void)
{
    int err = 0;

    if (SDL_hidapi_wasinit == SDL_FALSE) {
        return 0;
    }

#if HAVE_PLATFORM_BACKEND
    if (udev_ctx) {
        err = PLATFORM_hid_exit();
    }
#endif /* HAVE_PLATFORM_BACKEND */
#ifdef SDL_LIBUSB_DYNAMIC
    if (libusb_ctx.libhandle) {
        err |= LIBUSB_hid_exit(); /* Ehhhhh */
        SDL_UnloadObject(libusb_ctx.libhandle);
    }
#endif /* SDL_LIBUSB_DYNAMIC */
    return err;
}

struct hid_device_info HID_API_EXPORT * HID_API_CALL hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
#if HAVE_PLATFORM_BACKEND
    struct PLATFORM_hid_device_info *raw_devs = NULL;
    struct PLATFORM_hid_device_info *raw_dev;
#endif /* HAVE_PLATFORM_BACKEND */
    struct hid_device_info *devs = NULL, *last = NULL, *new_dev;
    SDL_bool bFound;

    if (SDL_hidapi_wasinit == SDL_FALSE) {
        hid_init();
    }

#if HAVE_PLATFORM_BACKEND
    if (udev_ctx) {
        raw_devs = PLATFORM_hid_enumerate(vendor_id, product_id);
    }
#endif /* HAVE_PLATFORM_BACKEND */

#ifdef SDL_LIBUSB_DYNAMIC
    if (libusb_ctx.libhandle) {
        struct LIBUSB_hid_device_info *usb_devs = LIBUSB_hid_enumerate(vendor_id, product_id);
        struct LIBUSB_hid_device_info *usb_dev;
        for (usb_dev = usb_devs; usb_dev; usb_dev = usb_dev->next) {
            bFound = SDL_FALSE;
#if HAVE_PLATFORM_BACKEND
            for (raw_dev = raw_devs; raw_dev; raw_dev = raw_dev->next) {
                if (usb_dev->vendor_id == raw_dev->vendor_id &&
                    usb_dev->product_id == raw_dev->product_id) {

                    bFound = SDL_TRUE;
                    break;
                }
            }
#endif

            if (!bFound) {
                new_dev = (struct hid_device_info*) SDL_malloc(sizeof(struct hid_device_info));
                LIBUSB_CopyHIDDeviceInfo(usb_dev, new_dev);

                if (last != NULL) {
                    last->next = new_dev;
                } else {
                    devs = new_dev;
                }
                last = new_dev;
            }
        }
        LIBUSB_hid_free_enumeration(usb_devs);
    }
#endif /* SDL_LIBUSB_DYNAMIC */

#if HAVE_PLATFORM_BACKEND
    if (udev_ctx) {
        for (raw_dev = raw_devs; raw_dev; raw_dev = raw_dev->next) {
            new_dev = (struct hid_device_info*) SDL_malloc(sizeof(struct hid_device_info));
            PLATFORM_CopyHIDDeviceInfo(raw_dev, new_dev);
            new_dev->next = NULL;

            if (last != NULL) {
                last->next = new_dev;
            } else {
                devs = new_dev;
            }
            last = new_dev;
        }
        PLATFORM_hid_free_enumeration(raw_devs);
    }
#endif /* HAVE_PLATFORM_BACKEND */

    return devs;
}

void  HID_API_EXPORT HID_API_CALL hid_free_enumeration(struct hid_device_info *devs)
{
    while (devs) {
        struct hid_device_info *next = devs->next;
        SDL_free(devs->path);
        SDL_free(devs->serial_number);
        SDL_free(devs->manufacturer_string);
        SDL_free(devs->product_string);
        SDL_free(devs);
        devs = next;
    }
}

HID_API_EXPORT hid_device * HID_API_CALL hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
    hid_device *pDevice = NULL;

    if (SDL_hidapi_wasinit == SDL_FALSE) {
        hid_init();
    }

#if HAVE_PLATFORM_BACKEND
    if (udev_ctx &&
        (pDevice = (hid_device*) PLATFORM_hid_open(vendor_id, product_id, serial_number)) != NULL) {

        HIDDeviceWrapper *wrapper = CreateHIDDeviceWrapper(pDevice, &PLATFORM_Backend);
        return WrapHIDDevice(wrapper);
    }
#endif /* HAVE_PLATFORM_BACKEND */
#ifdef SDL_LIBUSB_DYNAMIC
    if (libusb_ctx.libhandle &&
        (pDevice = (hid_device*) LIBUSB_hid_open(vendor_id, product_id, serial_number)) != NULL) {

        HIDDeviceWrapper *wrapper = CreateHIDDeviceWrapper(pDevice, &LIBUSB_Backend);
        return WrapHIDDevice(wrapper);
    }
#endif /* SDL_LIBUSB_DYNAMIC */
    return NULL;
}

HID_API_EXPORT hid_device * HID_API_CALL hid_open_path(const char *path, int bExclusive /* = false */)
{
    hid_device *pDevice = NULL;

    if (SDL_hidapi_wasinit == SDL_FALSE) {
        hid_init();
    }

#if HAVE_PLATFORM_BACKEND
    if (udev_ctx &&
        (pDevice = (hid_device*) PLATFORM_hid_open_path(path, bExclusive)) != NULL) {

        HIDDeviceWrapper *wrapper = CreateHIDDeviceWrapper(pDevice, &PLATFORM_Backend);
        return WrapHIDDevice(wrapper);
    }
#endif /* HAVE_PLATFORM_BACKEND */
#ifdef SDL_LIBUSB_DYNAMIC
    if (libusb_ctx.libhandle &&
        (pDevice = (hid_device*) LIBUSB_hid_open_path(path, bExclusive)) != NULL) {

        HIDDeviceWrapper *wrapper = CreateHIDDeviceWrapper(pDevice, &LIBUSB_Backend);
        return WrapHIDDevice(wrapper);
    }
#endif /* SDL_LIBUSB_DYNAMIC */
    return NULL;
}

int  HID_API_EXPORT HID_API_CALL hid_write(hid_device *device, const unsigned char *data, size_t length)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_write(wrapper->device, data, length);
}

int HID_API_EXPORT HID_API_CALL hid_read_timeout(hid_device *device, unsigned char *data, size_t length, int milliseconds)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_read_timeout(wrapper->device, data, length, milliseconds);
}

int  HID_API_EXPORT HID_API_CALL hid_read(hid_device *device, unsigned char *data, size_t length)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_read(wrapper->device, data, length);
}

int  HID_API_EXPORT HID_API_CALL hid_set_nonblocking(hid_device *device, int nonblock)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_set_nonblocking(wrapper->device, nonblock);
}

int HID_API_EXPORT HID_API_CALL hid_send_feature_report(hid_device *device, const unsigned char *data, size_t length)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_send_feature_report(wrapper->device, data, length);
}

int HID_API_EXPORT HID_API_CALL hid_get_feature_report(hid_device *device, unsigned char *data, size_t length)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_get_feature_report(wrapper->device, data, length);
}

void HID_API_EXPORT HID_API_CALL hid_close(hid_device *device)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    wrapper->backend->hid_close(wrapper->device);
    DeleteHIDDeviceWrapper(wrapper);
}

int HID_API_EXPORT_CALL hid_get_manufacturer_string(hid_device *device, wchar_t *string, size_t maxlen)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_get_manufacturer_string(wrapper->device, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_product_string(hid_device *device, wchar_t *string, size_t maxlen)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_get_product_string(wrapper->device, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_serial_number_string(hid_device *device, wchar_t *string, size_t maxlen)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_get_serial_number_string(wrapper->device, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_indexed_string(hid_device *device, int string_index, wchar_t *string, size_t maxlen)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_get_indexed_string(wrapper->device, string_index, string, maxlen);
}

HID_API_EXPORT const wchar_t* HID_API_CALL hid_error(hid_device *device)
{
    HIDDeviceWrapper *wrapper = UnwrapHIDDevice(device);
    return wrapper->backend->hid_error(wrapper->device);
}

#endif /* SDL_JOYSTICK_HIDAPI */
