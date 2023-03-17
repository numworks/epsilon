#ifndef BOOTLOADER_INTERFACE_STATIC_MESSAGES_H
#define BOOTLOADER_INTERFACE_STATIC_MESSAGES_H

namespace Bootloader {

class Messages {
public:
  constexpr static const char * mainTitle = "Upsilon Calculator";

  // Home menu
  constexpr static const char * homeTitle = "Select a slot";

  // Slots OS Type
  constexpr static const char * upsilonSlot = "-- Upsilon ";
  constexpr static const char * khiSlot = "-- Khi ";
  constexpr static const char * omegaSlot = "-- Omega ";
  constexpr static const char * epsilonSlot = "-- Epsilon ";
  constexpr static const char * invalidSlot = "X - Invalid slot";

  // Home Submenu
  constexpr static const char * homeSlotASubmenu = "1 - Slot A";
  constexpr static const char * homeSlotKhiSubmenu = "2 - Slot Khi";
  constexpr static const char * homeSlotBSubmenu = "3 - Slot B";
  constexpr static const char * homeInstallerSubmenu = "4 - Installer Mode";
  constexpr static const char * homeAboutSubmenu = "5 - About";
  constexpr static const char * homeRebootSubmenu = "6 - Reboot";

  // DFU menu
  constexpr static const char * dfuTitle = "Installer";

  constexpr static const char * dfuSlotsUpdate = "Waiting for Slots...";
  constexpr static const char * dfuBootloaderUpdate = "Waiting for Bootloader...";

  // Installer menu
  constexpr static const char * installerTitle = "Installer mode";

  constexpr static const char * installerText1 = "Please select a mode:";
  constexpr static const char * installerSlotsSubmenu = "1 - Flash Slots";
  constexpr static const char * installerBootloaderSubmenu = "2 - Flash Bootloader";

  // Bootloader Crash Handler
  constexpr static const char * bootloaderCrashTitle = "BOOTLOADER CRASH";

  constexpr static const char * bootloaderCrashMessage1 = "The bootloader has crashed.";
  constexpr static const char * bootloaderCrashMessage2 = "Please restart the calculator.";

  // Recovery menu
  constexpr static const char * recoveryTitle = "Recovery mode";

  constexpr static const char * recoveryMessage1 = "The bootloader has detected a crash.";
  constexpr static const char * recoveryMessage2 = "Plug the calculator to a device capable of";
  constexpr static const char * recoveryMessage3 = "accessing the internal storage.";
  constexpr static const char * recoveryMessage4 = "Press Back to continue.";
  constexpr static const char * recoveryMessage5 = "(you will not be able to recover your data !)";

  // Warning menu
  constexpr static const char * epsilonWarningTitle = "Epsilon Slot";

  constexpr static const char * epsilonWarningMessage1 = "!! WARNING !! ";
  constexpr static const char * epsilonWarningMessage2 = "This version of Epsilon";
  constexpr static const char * epsilonWarningMessage3 = "can lock the calculator.";
  constexpr static const char * epsilonWarningMessage4 = "Proceed the boot ?";
  constexpr static const char * epsilonWarningMessage5 = "EXE - Yes";
  constexpr static const char * epsilonWarningMessage6 = "BACK - No";

  // About menu
  constexpr static const char * aboutMenuTitle = "About";

  constexpr static const char * aboutMessage1 = "This is the bootloader of";
  constexpr static const char * aboutMessage2 = "the Upsilon Calculator.";
  constexpr static const char * aboutMessage3 = "It is used to install";
  constexpr static const char * aboutMessage4 = "and select the OS";
  constexpr static const char * aboutMessage5 = "to boot.";

  constexpr static const char * bootloaderVersion = "Version 1.0.4 - FREED0M.20.4";

  //USB NAMES
  constexpr static const char * usbUpsilonBootloader = "Upsilon Bootloader";
  constexpr static const char * usbUpsilonRecovery = "Upsilon Recovery";
  constexpr static const char * usbBootloaderUpdate = "Bootloader Update";

};

};

#endif
