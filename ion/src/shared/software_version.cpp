#include <ion.h>

#define STR_EXPAND(arg) #arg
#define STR(arg) STR_EXPAND(arg)

#ifndef PATCH_LEVEL
#error This file expects PATCH_LEVEL to be defined
#endif

const char * Ion::softwareVersion() {
  return STR(VERSION);
}

const char * Ion::patchLevel() {
  static char patchLevel[20] = {0};
  if (patchLevel[0] == 0) {
    strlcpy(patchLevel, STR(PATCH_LEVEL), 6+1);
  }
  return patchLevel;
}
