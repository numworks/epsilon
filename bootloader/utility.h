#ifndef _BOOTLOADER_ITOA_H_
#define _BOOTLOADER_ITOA_H_

/*
 * Taken from https://github.com/UpsilonNumworks/Upsilon/pull/205
 * 
 * Thanks to the Upsilon team, specifically:
 *  - devdl11
 *  - Yaya-Cout
 *  - Lauryy06
 */

namespace Bootloader {
  class Utility {
    public:
      static char * itoa(int value, char * result, int base);
      static int versionSum(const char * version, int length); 
  };
}

#endif
