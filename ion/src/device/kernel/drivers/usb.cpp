#include <shared/drivers/usb.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/keyboard.h>
#include <ion/src/shared/platform_info.h>
#include <shared/drivers/config/board.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

class SlotInfo {
public:
  SlotInfo() :
    m_header(Magic),
    m_footer(Magic)
  {
  }
  void update() {
    m_slotA = Board::isRunningSlotA();
    uint32_t slotStart = m_slotA ? Board::slotAUserlandStart() : Board::slotBUserlandStart();
    memcpy(&m_platformInfoBuffer, reinterpret_cast<PlatformInfo *>(slotStart + Ion::Device::Board::Config::UserlandHeaderOffset), sizeof(PlatformInfo));
  }
private:
  constexpr static uint32_t Magic = 0xEFEEDBBA;
  uint32_t m_header;
  bool m_slotA;
  uint8_t m_platformInfoBuffer[sizeof(PlatformInfo)];
  uint32_t m_footer;
};

void willExecuteDFU() {
  /* 1- Configure the keyboard to detect back event to leave DFU mode if the
   * Back key is pressed, the calculator unplugged or the USB core
   * soft-disconnected. */
  // TODO: interrupt on 'home', 'power'
  // TODO: try to use Keyboard::scan? Is it too slow?
  /* WARNING: we can't use hardware interruptions are their code is within the
   * kernel which might be being rewritter when USB is active... */
  Keyboard::activateRow(Keyboard::rowForKey(Ion::Keyboard::Key::Back));

  /* 2- Disable all interrupts
   * The interrupt service routines live in the Flash and could be overwritten
   * by garbage during a firmware upgrade opration, so we disable them. */
  Board::shutdownInterruptions();

  /* 3- Keep usefull information about the currently running slot */
  static SlotInfo slotInformation __attribute__((section(".slot_info")));
  slotInformation.update();
}

void didExecuteDFU() {
  Board::initInterruptions();
}

bool shouldInterruptDFU() {
  return Keyboard::columnIsActive(Keyboard::columnForKey(Ion::Keyboard::Key::Back));
}

}
}
}
