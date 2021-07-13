#include <shared/drivers/usb.h>

namespace Ion {
namespace Device {
namespace USB {

void willExecuteDFU() {}
void didExecuteDFU() {}
bool shouldInterruptDFU() { return false; }

}
}
}
