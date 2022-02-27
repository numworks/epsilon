#ifndef BOOTLOADER_TRAMPOLINE_H
#define BOOTLOADER_TRAMPOLINE_H

namespace Bootloader {

#define TRAMPOLINES_COUNT 13
extern void* Trampolines[TRAMPOLINES_COUNT];

#define CUSTOM_TRAMPOLINES_COUNT 2
extern void* CustomTrampolines[CUSTOM_TRAMPOLINES_COUNT];

}

#endif