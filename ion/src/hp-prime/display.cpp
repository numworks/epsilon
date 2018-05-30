#include <ion.h>
#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

uint16_t vidbuffer[320*240];

// Public Ion::Display methods

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  for (int y = r.top(); y <= r.bottom(); y++) {
    for (int x = r.left(); x <= r.right(); x++) {
      vidbuffer[y*320+x] = *pixels++;
    }
  }
}

void pushRectUniform(KDRect r, KDColor c) {
  for (int y = r.top(); y <= r.bottom(); y++) {
    for (int x = r.left(); x <= r.right(); x++) {
      vidbuffer[y*320+x] = c;
    }
  }
}

void pullRect(KDRect r, KDColor * pixels) {
  for (int y = r.top(); y <= r.bottom(); y++) {
    for (int x = r.left(); x <= r.right(); x++) {
      *pixels++ = KDColor::RGB16(vidbuffer[y*320+x]);
    }
  }
}

void waitForVBlank() {
  while (FIMD.VIDCON1()->getVSTATUS() != FIMD::VIDCON1::VSTATUS::Vsync);
}

}
}

// Private Ion::Display::Device methods

namespace Ion {
namespace Display {
namespace Device {

void init() {
  // Disable video circuits
  FIMD.VIDCON0()->setENVID(FIMD::VIDCON0::ENVID::DisableAtEndOfFrame);
  while (FIMD.VIDCON0()->getENVID() == FIMD::VIDCON0::ENVID::Enable);

  // Clear screen buffer
  memset(vidbuffer, 0xFF, sizeof(vidbuffer));

  // Perform black voodoo magic
  FIMD.VIDCON0()->set(0x00005270);
  FIMD.VIDCON1()->set(0x00000080);
  FIMD.VIDTCON0()->set(0x00110300);
  FIMD.VIDTCON1()->set(0x00401100);
  FIMD.VIDTCON2()->set(0x0007793f);
  FIMD.WINCON0()->set(0x00000000);
  FIMD.WINCON1()->set(0x00010015);
  FIMD.VIDOSD0A()->set(0x00000000);
  FIMD.VIDOSD0B()->set(0x0009f8ef);
  FIMD.VIDOSD1A()->set(0x00000000);
  FIMD.VIDOSD1B()->set(0x0009f8ef);
  FIMD.VIDW01ADD0()->set((uint32_t)&vidbuffer);
  FIMD.VIDW01ADD1()->set((uint32_t)&vidbuffer[320*240]);

  // Enable video circuits
  FIMD.VIDCON0()->setENVID(FIMD::VIDCON0::ENVID::Enable);

  // Enable backlight
  GPIO.GPBCON()->setMode(1, GPIO::GPBCON::Mode::Output);
  GPIO.GPBDAT()->setBitRange(1, 1, true);
}

}
}
}
