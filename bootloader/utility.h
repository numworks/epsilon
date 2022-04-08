#ifndef _BOOTLOADER_ITOA_H_
#define _BOOTLOADER_ITOA_H_

namespace Utility {

    static char * itoa(int value, char * result, int base);
    static int versionSum(const char * version, int length);
}

#endif
