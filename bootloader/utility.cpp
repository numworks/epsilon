#include <bootloader/utility.h>
#include <string.h>

/*
 * Taken from https://github.com/UpsilonNumworks/Upsilon/pull/205
 * 
 * Thanks to the Upsilon team, specifically:
 *  - devdl11
 *  - Yaya-Cout
 *  - Lauryy06
 */

int Bootloader::Utility::versionSum(const char * version, int length) {
    int sum = 0;
    for (int i  = 0; i < length; i++) {
        sum += version[i] * (strlen(version) * 100 - i * 10);
    }
    return sum;
}
