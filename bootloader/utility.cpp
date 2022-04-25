#include <bootloader/utility.h>
#include <string.h>

int Utility::versionSum(const char * version, int length) {
  int sum = 0;
  for (int i  = 0; i < length; i++) {
    sum += version[i] * (strlen(version) * 100 - i * 10);
  }
  return sum;
}
