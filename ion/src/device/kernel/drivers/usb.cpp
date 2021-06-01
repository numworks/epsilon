#include <kernel/drivers/usb.h>
#include <kernel/drivers/keyboard.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

SlotInfo * slotInfo() {
  static SlotInfo slotInformation __attribute__((section(".slot_info")));
  return &slotInformation;
}

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
  slotInfo()->update();
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
