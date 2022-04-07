#ifndef BOOTLOADER_INTERFACE_MESSAGES_H
#define BOOTLOADER_INTERFACE_MESSAGES_H

namespace Bootloader {

class Messages {
public:
  // TODO: Remove it when this fork will be updated
  #define UPSILON_VERSION "1.0.0-dev"
  #ifdef UPSILON_VERSION
  constexpr static const char * mainTitle = "Upsilon Calculator";
  #elif defined OMEGA_VERSION
  constexpr static const char * mainTitle = "Omega Calculator";
  #else
  constexpr static const char * mainTitle = "NumWorks Calculator";
  #endif
  constexpr static const char * mainMenuTitle = "Select a slot";
  constexpr static const char * upsilon = "-- Upsilon ";
  constexpr static const char * khi = "-- Khi ";
  constexpr static const char * omega = "-- Omega ";
  constexpr static const char * epsilon = "-- Epsilon ";
  constexpr static const char * invalidSlot = "X - Invalid slot";
  constexpr static const char * dfuText = "4 - Installer Mode";
  constexpr static const char * aboutText = "5 - About";
  constexpr static const char * rebootText = "6 - Reboot";
  constexpr static const char * dfuSubtitle = "Waiting for Slots...";
  constexpr static const char * crashTitle = "BOOTLOADER CRASH";
  constexpr static const char * crashText1 = "The bootloader has crashed.";
  constexpr static const char * crashText2 = "Please restart the calculator.";
  constexpr static const char * recoveryTitle = "Recovery mode";
  constexpr static const char * recoveryText1 = "The bootloader has detected a crash.";
  constexpr static const char * recoveryText2 = "Plug the calculator to a device capable of";
  constexpr static const char * recoveryText3 = "accessing the internal storage.";
  constexpr static const char * recoveryText4 = "Press Back to continue.";
  constexpr static const char * recoveryText5 = "(you will not be able to recover your data !)";
  constexpr static const char * installerSelectionTitle = "Installer Mode";
  constexpr static const char * installerText1 = "Please select a mode:";
  constexpr static const char * installerText2 = "1. Flash slots";
  constexpr static const char * installerText3 = "2. Flash the bootloader";
  constexpr static const char * bootloaderSubtitle = "Waiting for the bootloader...";
  constexpr static const char * epsilonWarningTitle = "Epsilon Slot";
  constexpr static const char * epsilonWarningText1 = "!! WARNING !! ";
  constexpr static const char * epsilonWarningText2 = "This version of epsilon";
  constexpr static const char * epsilonWarningText3 = "can lock the calculator.";
  constexpr static const char * epsilonWarningText4 = "Proceed the boot ?";
  constexpr static const char * epsilonWarningText5 = "EXE - Yes";
  constexpr static const char * epsilonWarningText6 = "BACK - No";
  constexpr static const char * bootloaderVersion = "Version 1.0.0 - FREEDOM";

  //USB NAMES
  
  constexpr static const char * upsilonBootloader = "Upsilon Bootloader";
  constexpr static const char * upsilonRecovery = "Upsilon Recovery";
  constexpr static const char * bootloaderUpdate = "Bootloader Update";

};

};

#endif