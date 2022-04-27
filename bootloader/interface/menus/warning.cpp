#include "warning.h"
#include <bootloader/slots/slot.h>
#include <bootloader/boot.h>

Bootloader::WarningMenu::WarningMenu() : Menu(KDColorWhite, KDColorRed, Messages::epsilonWarningTitle, Messages::mainTitle, false, 3) {
  setup();
}

bool proceed() {
  Bootloader::Boot::bootSelectedSlot();
  return true;
}

bool backoff() {
  if (Bootloader::Boot::config()->slot() != nullptr) {
    Bootloader::Boot::config()->clearSlot();
  }
  return true;
} 

void Bootloader::WarningMenu::setup() {
  m_defaultColumns[0] = Column(Messages::epsilonWarningMessage1, k_small_font, 0, true);
  m_defaultColumns[1] = Column(Messages::epsilonWarningMessage2, k_small_font, 0, true);
  m_defaultColumns[2] = Column(Messages::epsilonWarningMessage3, k_small_font, 0, true);
  m_defaultColumns[3] = Column(Messages::epsilonWarningMessage4, k_small_font, 0, true);
  m_defaultColumns[4] = Column(Messages::epsilonWarningMessage5, Ion::Keyboard::Key::EXE, k_small_font, 0, true, &proceed);
  m_defaultColumns[5] = Column(Messages::epsilonWarningMessage6, Ion::Keyboard::Key::Back, k_small_font, 0, true, &backoff);
  
  m_columns[0] = ColumnBinder(&m_defaultColumns[0]);
  m_columns[1] = ColumnBinder(&m_defaultColumns[1]);
  m_columns[2] = ColumnBinder(&m_defaultColumns[2]);
  m_columns[3] = ColumnBinder(&m_defaultColumns[3]);
  m_columns[4] = ColumnBinder(&m_defaultColumns[4]);
  m_columns[5] = ColumnBinder(&m_defaultColumns[5]);
}
