#include <ion.h>

namespace Flasher {
namespace Display {

void init() {
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0xFFFF00));
}

}
}
