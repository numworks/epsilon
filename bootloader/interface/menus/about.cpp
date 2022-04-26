#include "about.h"
#include <bootloader/interface/static/messages.h>

Bootloader::AboutMenu::AboutMenu() : Menu(KDColorBlack, KDColorWhite, Messages::aboutMenuTitle, Messages::bootloaderVersion) {
    setup();
}

void Bootloader::AboutMenu::setup() {
    m_default_columns[0] = Column(Messages::aboutMessage1, k_small_font, 0, true);
    m_default_columns[1] = Column(Messages::aboutMessage2, k_small_font, 0, true);
    m_default_columns[2] = Column(Messages::aboutMessage3, k_small_font, 0, true);
    m_default_columns[3] = Column(Messages::aboutMessage4, k_small_font, 0, true);
    m_default_columns[4] = Column(Messages::aboutMessage5, k_small_font, 0, true);

    m_columns[0] = ColumnBinder(&m_default_columns[0]);
    m_columns[1] = ColumnBinder(&m_default_columns[1]);
    m_columns[2] = ColumnBinder(&m_default_columns[2]);
    m_columns[3] = ColumnBinder(&m_default_columns[3]);
    m_columns[4] = ColumnBinder(&m_default_columns[4]);
}
