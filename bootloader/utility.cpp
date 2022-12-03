#include <bootloader/utility.h>
#include <string.h>

// This function takes a pointer to a string (version) and the size of the
// string (versionSize) and returns an integer representing the version.
// Example: "1.2.3" will return 10203.
//          "1.0.1-dev" will return 10001.
int Utility::versionSum(const char * version, int length) {
  int sum = 0;
  int currentNumber = 0;
  // List of numbers that are allowed in a version
  const char * allowedNumbers = "0123456789";
  for (int i = 0; i < length; i++) {
    if (version[i] == '.') {
      sum = sum * 100 + currentNumber;
      currentNumber = 0;
    } else if (strchr(allowedNumbers, version[i]) != nullptr) {
      currentNumber = currentNumber * 10 + (version[i] - '0');
    } else {
      // We found a character that is not a number or a dot, so we stop
      break;
    }
  }
  sum = sum * 100 + currentNumber;
  return sum;
}
