#include <drivers/svcall.h>
#include <ion.h>

namespace Ion {

uint32_t SVC_ATTRIBUTES devicePcbVersion() {
  SVC_RETURNING_R0(SVC_PCB_VERSION, uint32_t)
}

const char * pcbVersion() {
  constexpr int pcbVersionLength = 5; // xx.yy
  static char pcbVer[pcbVersionLength + 1] = {'\0'};
  if (pcbVer[0] == '\0') {
    uint32_t ver = devicePcbVersion();
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
    pcbVer[pcbVersionLength] = 0;
  }
  return pcbVer;
}

}
