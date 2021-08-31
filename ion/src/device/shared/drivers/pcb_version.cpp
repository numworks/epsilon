#include <ion.h>
#include "board.h"

namespace Ion {

const char * pcbVersion() {
  constexpr int pcbVersionLength = 5; // xx.yy
  static char pcbVer[pcbVersionLength] = {'\0'};
  if (pcbVer[0] == '\0') {
    Device::Board::PCBVersion ver = Device::Board::pcbVersion();
    /* As PCB version only uses 4 chars, value should be at most 9999. */
    assert(ver < 10000);
    for (int i = pcbVersionLength - 1; i >= 0; i--) {
      if (i == 2) {
        pcbVer[i] = '.';
      } else {
        pcbVer[i] = '0' + ver % 10;
        ver /= 10;
      }
    }
  }
  return pcbVer;
}

}
