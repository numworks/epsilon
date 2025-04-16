#include <omg/unreachable.h>
#include <shared/drivers/usb.h>
#include <shared/usb/dfu_interfaces.h>
#include <shared/usb/dfu_interfaces_helpers.h>

namespace Ion {
namespace Device {
namespace USB {

constexpr InterfaceDescriptor k_SRAMDescriptor =
    helper(1, &k_dfuFunctionalDescriptor);
constexpr InterfaceDescriptor k_flashDescriptor = helper(0, &k_SRAMDescriptor);

const InterfaceDescriptor* firstInterfaceDescriptor() {
  return &k_flashDescriptor;
}

int interfaceDescriptorsTotalLength() {
  return k_flashDescriptor.BLength() + k_SRAMDescriptor.BLength() +
         k_dfuFunctionalDescriptor.BLength();
}

const DFUInterfaceBackend* interface(int index) {
  switch (index) {
    case 0: {
      constexpr static DFUFlashBackend k_flashBackend(0, UINT32_MAX);
      return &k_flashBackend;
    }
    case 1: {
      constexpr static DFURAMBackend k_RAMBackend(0, UINT32_MAX);
      return &k_RAMBackend;
    }
    default:
      OMG::unreachable();
  }
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
