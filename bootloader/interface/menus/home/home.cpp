#include "home.h"
#include <bootloader/slot.h>
#include <bootloader/interface/menus/about/about.h>

Bootloader::AboutMenu * Bootloader::HomeMenu::aboutMenu() {
  static AboutMenu * aboutMenu = new AboutMenu();
  return aboutMenu;
}

Bootloader::HomeMenu::HomeMenu() : Menu(KDColorBlack, KDColorWhite, Messages::mainMenuTitle, Messages::mainTitle) {
    setup();
}

bool slotA_submenu() {
  if (Bootloader::Slot::isFullyValid(Bootloader::Slot::A())) {
  
  }
  return false;
}

bool slotKhi_submenu() {
  if (Bootloader::Slot::isFullyValid(Bootloader::Slot::Khi())) {
    
  }
  return false;
}

bool slotB_submenu() {
  if (Bootloader::Slot::isFullyValid(Bootloader::Slot::B())) {
    
  }
  return false;
}

bool installer_submenu() {
  return false;
}

bool about_submenu() {
  Bootloader::HomeMenu::aboutMenu()->open();
  return true;
}

const char * Bootloader::HomeMenu::slotA_text() {
    return Slot::isFullyValid(Slot::A()) ? "1 - Slot A" : Messages::invalidSlot;
}

const char * Bootloader::HomeMenu::slotKhi_text() {
    return Slot::isFullyValid(Slot::Khi()) ? "2 - Slot Khi" : Messages::invalidSlot;
}

const char * Bootloader::HomeMenu::slotB_text() {
    return Slot::isFullyValid(Slot::B()) ? "3 - Slot B" : Messages::invalidSlot;
}

void Bootloader::HomeMenu::setup() {
    m_colomns[0] = Colomn(slotA_text(), Ion::Keyboard::Key::One, k_small_font, 30, false, &slotA_submenu);
    m_colomns[1] = Colomn(slotKhi_text(), Ion::Keyboard::Key::Two, k_small_font, 30, false, &slotKhi_submenu);
    m_colomns[2] = Colomn(slotB_text(), Ion::Keyboard::Key::Three, k_small_font, 30, false, &slotB_submenu);
    m_colomns[3] = Colomn("4- installer", Ion::Keyboard::Key::Four, k_small_font, 30, false, &installer_submenu);
    m_colomns[4] = Colomn("5- about", Ion::Keyboard::Key::Five, k_small_font, 30, false, &about_submenu);
}