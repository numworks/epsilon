#include "slot_recovery.h"
#include <ion.h>

Bootloader::SlotRecoveryMenu::SlotRecoveryMenu(USBData * usb) : Menu(KDColorBlack, KDColorWhite, Messages::recoveryTitle, Messages::mainTitle), m_data(usb) {
    setup();
}

void Bootloader::SlotRecoveryMenu::setup() {
  m_default_columns[0] = Column(Messages::recoveryMessage1, k_small_font, 0, true);
  m_default_columns[1] = Column(Messages::recoveryMessage2, k_small_font, 0, true);
  m_default_columns[2] = Column(Messages::recoveryMessage3, k_small_font, 0, true);
  m_default_columns[3] = Column(Messages::recoveryMessage4, k_small_font, 0, true);
  m_default_columns[4] = Column(Messages::recoveryMessage5, k_small_font, 0, true);
  
  m_columns[0] = ColumnBinder(&m_default_columns[0]);
  m_columns[1] = ColumnBinder(&m_default_columns[1]);
  m_columns[2] = ColumnBinder(&m_default_columns[2]);
  m_columns[3] = ColumnBinder(&m_default_columns[3]);
  m_columns[4] = ColumnBinder(&m_default_columns[4]);
}

void Bootloader::SlotRecoveryMenu::post_open() {
  // We override the open method
  for (;;) {
    Ion::USB::enable();
    do {
      uint64_t scan = Ion::Keyboard::scan();
      if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
        Ion::USB::disable();
        forceExit();
        return;
      } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
        Ion::Power::standby();
        return;
      }
    } while (!Ion::USB::isEnumerated());
    Ion::USB::DFU(true, (void *)m_data);
  }
}
