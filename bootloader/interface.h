#ifndef BOOTLOADER_INTERFACE
#define BOOTLOADER_INTERFACE

#include <stdint.h>
#include <kandinsky/context.h>
#include <escher/image.h>

namespace Bootloader {

class Interface {

private:
  static void drawImage(KDContext * ctx, const Image * image, int offset);

public:
  static void drawLoading();
  static void drawHeader();
  static void drawMenu();
  static void drawFlasher();
  static void drawAbout();
  static void drawCrash(const char * error);
  static void drawRecovery();
  static void drawInstallerSelection();
  static void drawBLUpdate();
  static void drawEpsilonAdvertisement();

};

}

#endif