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

// Public Ion::Backlight methods

namespace Ion {
namespace Backlight {

void setBrightness(uint8_t b) {
  PWM.TCNTB1()->set(256/8);
  PWM.TCMPB1()->set(b/8);

  PWM.TCON()->setTimer(1, PWM::TCON::AUTORELOAD|PWM::TCON::MANUAL_UPDATE);
  PWM.TCON()->setTimer(1, PWM::TCON::AUTORELOAD|PWM::TCON::ENABLE);
}

uint8_t brightness() {
  return PWM.TCMPB1()->get() * 8;
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

  // TIM1 is the PWM for the display
  PWM.TCFG1()->setMode(1, PWM::TCFG1::Mode::MUX_1_16);

  // Enable backlight
  GPIO.GPBCON()->setMode(1, GPIO::GPBCON::Mode::Alternate0);
  Ion::Backlight::setBrightness(255);
}

}
}
}
