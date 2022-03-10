#include <drivers/trampoline.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Trampoline {

uint32_t address(int index) {
  return 0x0020E000 + sizeof(void *) * index;
}

}
}
}
