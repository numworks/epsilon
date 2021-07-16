#include <drivers/reset.h>
#include <ion/reset.h>

namespace Ion {
namespace Reset {

void core() {
  Ion::Device::Reset::core();
}


}
}
