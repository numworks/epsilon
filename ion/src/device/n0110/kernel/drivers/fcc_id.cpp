#include <kernel/drivers/fcc_id.h>
#include <ion.h>
#include <string.h>

void Ion::Device::fccId(char * buffer) {
  strlcpy(buffer, "2ALWP-N0110", FccIDLength + 1);
}
