#include <drivers/flash.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Flash {

bool SectorIsWritable(int i) {
  assert(i >= 0 && i < TotalNumberOfSectors());
  return true;
}

}
}
}
