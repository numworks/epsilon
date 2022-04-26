#include "installer.h"
#include <bootloader/interface/static/messages.h>
#include <bootloader/usb_data.h>

#include <bootloader/interface/menus/dfu.h>

Bootloader::DfuMenu * Bootloader::InstallerMenu::SlotsDFU() {
    USBData data = USBData::DEFAULT();
    static DfuMenu * slotsDfu = new DfuMenu(Messages::dfuSlotsUpdate, &data);
    return slotsDfu;
}

Bootloader::DfuMenu * Bootloader::InstallerMenu::BootloaderDFU() {
    USBData data = USBData::BOOTLOADER_UPDATE();
    static DfuMenu * bootloaderDfu = new DfuMenu(Messages::dfuBootloaderUpdate, &data);
    return bootloaderDfu;
}

Bootloader::InstallerMenu::InstallerMenu() : Menu(KDColorBlack, KDColorWhite, Messages::installerTitle, Messages::mainTitle) {
    setup();
}

bool slots_submenu() {
    Bootloader::InstallerMenu::SlotsDFU()->open();
    return true;
}

bool bootloader_submenu() {
    Bootloader::InstallerMenu::BootloaderDFU()->open();
    return true;
}

void Bootloader::InstallerMenu::setup() {
    m_default_columns[0] = Column(Messages::installerText1, k_large_font, 0, true);
    m_default_columns[1] = Column(Messages::installerSlotsSubmenu, Ion::Keyboard::Key::One, k_small_font, 30, false, &slots_submenu);
    m_default_columns[2] = Column(Messages::installerBootloaderSubmenu, Ion::Keyboard::Key::Two, k_small_font, 30, false, &bootloader_submenu);

    m_columns[0] = ColumnBinder(&m_default_columns[0]);
    m_columns[1] = ColumnBinder(&m_default_columns[1]);
    m_columns[2] = ColumnBinder(&m_default_columns[2]);
}
