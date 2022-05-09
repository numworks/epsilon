#ifndef ION_DEVICE_SHARED_USB_H
#define ION_DEVICE_SHARED_USB_H

#include <config/board.h>
#include <drivers/kernel_header.h>
#include <drivers/userland_header.h>
#include <ion/events.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace USB {

using namespace Regs;

namespace Config {

/* On the STM32F730, PA9 does not actually support alternate function 10.
 * However, because of the wiring of the USB connector on old N0110, detection
 * of when the device is plugged required the use of this undocumented setting.
 * After the revision of the USB connector and ESD protection, we can now
 * follow the specification and configure the Vbus pin as a floating-input GPIO.
 */
constexpr static AFGPIOPin VbusPin = AFGPIOPin(GPIOA, 9, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

constexpr static AFGPIOPin DmPin = AFGPIOPin(GPIOA, 11, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DpPin = AFGPIOPin(GPIOA, 12, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

}

/* When entering DFU mode, we update this SlotInfo with a pointer to the current
 * PlatformInfo of the binary being run. The slot info is located at the very
 * beginning of the sRAM. */
class SlotInfo {
public:
  SlotInfo() :
    m_header(Magic),
    m_footer(Magic)
  {
  }
  void updateKernelHeader();
  void updateUserlandHeader();
  KernelHeader * kernelHeader() const { return m_kernelHeaderAddress; }
private:
  constexpr static uint32_t Magic = 0xEFEEDBBA;
  uint32_t m_header;
  KernelHeader * m_kernelHeaderAddress;
  UserlandHeader * m_userlandHeaderAddress;
  uint32_t m_footer;
};

void init();
void initGPIO();
void initOTG();

void shutdown();
void shutdownGPIO();
void shutdownOTG();

void willExecuteDFU();
void didExecuteDFU();
Ion::Events::Event shouldInterruptDFU();
const char * stringDescriptor();
SlotInfo * slotInfo();

}
}
}

#endif
