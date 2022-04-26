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

const char * Bootloader::HomeMenu::slotA_text() {
  return Slot::isFullyValid(Slot::A()) ? Messages::homeSlotASubmenu : Messages::invalidSlot;
}

const char * Bootloader::HomeMenu::slotA_kernel_text() {
  return Slot::isFullyValid(Slot::A()) ? Slot::A().kernelHeader()->patchLevel() : nullptr;
}

const char * Bootloader::HomeMenu::slotA_version_text() {
  return Slot::isFullyValid(Slot::A()) ? Slot::A().userlandHeader()->isOmega() && Slot::A().userlandHeader()->isUpsilon() ? Slot::A().userlandHeader()->upsilonVersion() : Slot::A().userlandHeader()->isOmega() ? Slot::A().userlandHeader()->omegaVersion() : Slot::A().kernelHeader()->version() : nullptr;
}

const char * Bootloader::HomeMenu::slotA_os_text() {
  if (Slot::isFullyValid(Slot::A())) {
    if (Slot::A().userlandHeader()->isOmega() && Slot::A().userlandHeader()->isUpsilon()) {
      return Messages::upsilonSlot;
    } else if (Slot::A().userlandHeader()->isOmega()) {
      return Messages::omegaSlot;
    } else {
      return Messages::epsilonSlot;
    }
  }
  return nullptr;
}

const char * Bootloader::HomeMenu::slotKhi_text() {
    return Slot::isFullyValid(Slot::Khi()) ? Messages:: homeSlotKhiSubmenu : Messages::invalidSlot;
}

const char * Bootloader::HomeMenu::slotKhi_kernel_text() {
  return Slot::isFullyValid(Slot::Khi()) ? Slot::Khi().kernelHeader()->patchLevel() : nullptr;
}

const char * Bootloader::HomeMenu::slotKhi_os_text() {
  if (Slot::isFullyValid(Slot::Khi())) {
    if (Slot::Khi().userlandHeader()->isOmega() && Slot::Khi().userlandHeader()->isUpsilon()) {
      return Messages::upsilonSlot;
    } else if (Slot::Khi().userlandHeader()->isOmega()) {
      return Messages::omegaSlot;
    } else {
      return Messages::epsilonSlot;
    }
  }
  return nullptr;
}

const char * Bootloader::HomeMenu::slotKhi_version_text() {
  return Slot::isFullyValid(Slot::Khi()) ? Slot::Khi().userlandHeader()->isOmega() && Slot::Khi().userlandHeader()->isUpsilon() ? Slot::Khi().userlandHeader()->upsilonVersion() : Slot::Khi().userlandHeader()->isOmega() ? Slot::Khi().userlandHeader()->omegaVersion() : Slot::Khi().kernelHeader()->version() : nullptr;
}

const char * Bootloader::HomeMenu::slotB_text() {
    return Slot::isFullyValid(Slot::B()) ? Messages::homeSlotBSubmenu : Messages::invalidSlot;
}

const char * Bootloader::HomeMenu::slotB_kernel_text() {
  return Slot::isFullyValid(Slot::B()) ? Slot::B().kernelHeader()->patchLevel() : nullptr;
}

const char * Bootloader::HomeMenu::slotB_os_text() {
  if (Slot::isFullyValid(Slot::B())) {
    if (Slot::B().userlandHeader()->isOmega() && Slot::B().userlandHeader()->isUpsilon()) {
      return Messages::upsilonSlot;
    } else if (Slot::B().userlandHeader()->isOmega()) {
      return Messages::omegaSlot;
    } else {
      return Messages::epsilonSlot;
    }
  }
  return nullptr;
}

const char * Bootloader::HomeMenu::slotB_version_text() {
  return Slot::isFullyValid(Slot::B()) ? Slot::B().userlandHeader()->isOmega() && Slot::B().userlandHeader()->isUpsilon() ? Slot::B().userlandHeader()->upsilonVersion() : Slot::B().userlandHeader()->isOmega() ? Slot::B().userlandHeader()->omegaVersion() : Slot::B().kernelHeader()->version() : nullptr;
}

void Bootloader::HomeMenu::setup() {
    m_slot_columns[0] = SlotColumn(slotA_text(), slotA_kernel_text(), slotA_os_text(), slotA_version_text(),  Ion::Keyboard::Key::One, k_small_font, 10, false, &slotA_submenu);
    m_slot_columns[1] = SlotColumn(slotKhi_text(), slotKhi_kernel_text(), slotKhi_os_text(), slotKhi_version_text(), Ion::Keyboard::Key::Two, k_small_font, 10, false, &slotKhi_submenu);
    m_slot_columns[2] = SlotColumn(slotB_text(), slotB_kernel_text(), slotB_os_text(), slotB_version_text(), Ion::Keyboard::Key::Three, k_small_font, 10, false, &slotB_submenu);
    m_default_columns[0] = Column(Messages::homeInstallerSubmenu, Ion::Keyboard::Key::Four, k_small_font, 10, false, &installer_submenu);
    m_default_columns[1] = Column(Messages::homeAboutSubmenu, Ion::Keyboard::Key::Five, k_small_font, 10, false, &about_submenu);
    

    m_columns[0] = ColumnBinder(&m_slot_columns[0]);
    m_columns[1] = ColumnBinder(&m_slot_columns[1]);
    m_columns[2] = ColumnBinder(&m_slot_columns[2]);
    m_columns[3] = ColumnBinder(&m_default_columns[0]);
    m_columns[4] = ColumnBinder(&m_default_columns[1]);

}
