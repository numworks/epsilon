#include "dfu.h"
#include <bootloader/boot.h>
#include <ion.h>

Bootloader::DfuMenu::DfuMenu(const char * text, const USBData * data) : Menu(KDColorBlack, KDColorWhite, Messages::dfuTitle, Messages::mainTitle), m_submenuText(text), m_data(data) {
  setup();
}

void Bootloader::DfuMenu::setup() {
  m_defaultColumns[0] = Column(m_submenuText, k_small_font, 0, true);

  m_columns[0] = ColumnBinder(&m_defaultColumns[0]);
}

void Bootloader::DfuMenu::postOpen() {
  // We override the open method
  if (!m_data->getData().isProtectedInternal() && m_data->getData().isProtectedExternal()) {
    // Because we want to flash the internal, we will jump into the stm32 bootloader
    Bootloader::Boot::jumpToInternalBootloader();
    return; // We never reach this point
  }
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
