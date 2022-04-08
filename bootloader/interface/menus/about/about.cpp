#include "about.h"

Bootloader::AboutMenu::AboutMenu() : Menu(KDColorBlack, KDColorWhite, Messages::aboutMenuTitle, Messages::bootloaderVersion) {
    setup();
}

void Bootloader::AboutMenu::setup() {
    m_colomns[0] = Colomn("This is the bootloader of", k_small_font, 0, true);
    m_colomns[1] = Colomn("the Upsilon Calculator.", k_small_font, 0, true);
    m_colomns[2] = Colomn("It is used to install", k_small_font, 0, true);
    m_colomns[3] = Colomn("and select the OS", k_small_font, 0, true);
    m_colomns[4] = Colomn("to boot.", k_small_font, 0, true);
}