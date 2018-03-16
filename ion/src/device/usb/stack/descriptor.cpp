#include "descriptor.h"
#include <string.h>

namespace Ion {
namespace USB {
namespace Device {

void Descriptor::push(Channel * c) const {
  c->push(bLength());
  c->push(m_bDescriptorType);
}

}
}
}
