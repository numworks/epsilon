#include "home.h"
#include <bootloader/boot.h>
#include <bootloader/slots/slot.h>
#include <bootloader/interface/menus/about.h>
#include <stdlib.h>

Bootloader::AboutMenu * Bootloader::HomeMenu::aboutMenu() {
  static AboutMenu * aboutMenu = new AboutMenu();
  return aboutMenu;
}

Bootloader::InstallerMenu * Bootloader::HomeMenu::installerMenu() {
  static InstallerMenu * installerMenu = new InstallerMenu();
  return installerMenu;
}

Bootloader::HomeMenu::HomeMenu() : Menu(KDColorBlack, KDColorWhite, Messages::homeTitle, Messages::mainTitle) {
  setup();
}

bool slotA_submenu() {
  if (Bootloader::Slot::isFullyValid(Bootloader::Slot::A())) {
    Bootloader::Boot::bootSlot(Bootloader::Slot::A());
    return true;
  }
  return false;
}

bool slotKhi_submenu() {
  if (Bootloader::Slot::isFullyValid(Bootloader::Slot::Khi())) {
    Bootloader::Boot::bootSlot(Bootloader::Slot::Khi());
    return true;
  }
  return false;
}

bool slotB_submenu() {
  if (Bootloader::Slot::isFullyValid(Bootloader::Slot::B())) {
    Bootloader::Boot::bootSlot(Bootloader::Slot::B());
    return true;
  }
  return false;
}

bool installer_submenu() {
  Bootloader::HomeMenu::installerMenu()->open();
  return true;
}

bool about_submenu() {
  Bootloader::HomeMenu::aboutMenu()->open();
  return true;
}

const char * Bootloader::HomeMenu::getSlotOsText(Slot slot) {
  if (Slot::isFullyValid(slot)) {
    if (slot.userlandHeader()->isOmega() && slot.userlandHeader()->isUpsilon()) {
      return Messages::upsilonSlot;
    } else if (slot.userlandHeader()->isOmega() && slot.kernelHeader()->patchLevel()[0] != '\0') {
      return Messages::omegaSlot;
    } else if (slot.userlandHeader()->isOmega()) {
      return Messages::khiSlot;
    } else {
      return Messages::epsilonSlot;
    }
  }
  return nullptr;
}

const char * Bootloader::HomeMenu::getSlotText(Slot slot) {
  if(Slot::isFullyValid(slot)) {
    if (slot.address() == Slot::A().address()) {
        return Messages::homeSlotASubmenu;
    } else if (slot.address() == Slot::Khi().address()) {
        return Messages::homeSlotKhiSubmenu;
    } else if (slot.address() == Slot::B().address()) {
        return Messages::homeSlotBSubmenu;
    }
  }
  return Messages::invalidSlot;
}

const char * Bootloader::HomeMenu::getKernelText(Slot slot) {
  return Slot::isFullyValid(slot) ? slot.kernelHeader()->patchLevel() : nullptr;
}

const char * Bootloader::HomeMenu::getVersionText(Slot slot) {
  return Slot::isFullyValid(slot) ? slot.userlandHeader()->isOmega() && slot.userlandHeader()->isUpsilon() ? slot.userlandHeader()->upsilonVersion() : slot.userlandHeader()->isOmega() ? slot.userlandHeader()->omegaVersion() : slot.kernelHeader()->version() : nullptr;
}

void Bootloader::HomeMenu::setup() {
  Slot A = Slot::A();
  Slot Khi = Slot::Khi();
  Slot B = Slot::B();

  m_slotColumns[0] = SlotColumn(getSlotText(A), getKernelText(A), getSlotOsText(A), getVersionText(A),  Ion::Keyboard::Key::One, k_small_font, 10, false, &slotA_submenu);
  m_slotColumns[1] = SlotColumn(getSlotText(Khi), getKernelText(Khi), getSlotOsText(Khi), getVersionText(Khi), Ion::Keyboard::Key::Two, k_small_font, 10, false, &slotKhi_submenu);
  m_slotColumns[2] = SlotColumn(getSlotText(B), getKernelText(B), getSlotOsText(B), getVersionText(B), Ion::Keyboard::Key::Three, k_small_font, 10, false, &slotB_submenu);
  m_defaultColumns[0] = Column(Messages::homeInstallerSubmenu, Ion::Keyboard::Key::Four, k_small_font, 10, false, &installer_submenu);
  m_defaultColumns[1] = Column(Messages::homeAboutSubmenu, Ion::Keyboard::Key::Five, k_small_font, 10, false, &about_submenu);

  m_columns[0] = ColumnBinder(&m_slotColumns[0]);
  m_columns[1] = ColumnBinder(&m_slotColumns[1]);
  m_columns[2] = ColumnBinder(&m_slotColumns[2]);
  m_columns[3] = ColumnBinder(&m_defaultColumns[0]);
  m_columns[4] = ColumnBinder(&m_defaultColumns[1]);
}
