#include "crash.h"

Bootloader::CrashMenu::CrashMenu(const char * err) : Menu(KDColorBlack, KDColorWhite, Bootloader::Messages::bootloaderCrashTitle, Bootloader::Messages::mainTitle), m_error(err) {
  setup();
}

void Bootloader::CrashMenu::setup() {
  m_defaultColumns[0] = Column(m_error, k_large_font, 0, true);
  m_defaultColumns[1] = Column(Bootloader::Messages::bootloaderCrashMessage1, k_small_font, 0, true);
  m_defaultColumns[2] = Column(Bootloader::Messages::bootloaderCrashMessage2, k_small_font, 0, true);

  m_columns[0] = ColumnBinder(&m_defaultColumns[0]);
  m_columns[1] = ColumnBinder(&m_defaultColumns[1]);
  m_columns[2] = ColumnBinder(&m_defaultColumns[2]);
}

void Bootloader::CrashMenu::postOpen() {
  // We override the open method
  for (;;) {
    // Infinite loop
  }
}
