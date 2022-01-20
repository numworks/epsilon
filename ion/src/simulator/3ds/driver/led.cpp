#include <ion/led.h>
#include <ion/battery.h>
#include <ion/usb.h>
#include <ion/exam_mode.h>

#include <string.h>
#include <3ds.h>
#include "common.h"

static KDColor sLedColor = KDColorBlack;

namespace Ion {
namespace LED {

KDColor getColor() {
  return sLedColor;
}

void setColor(KDColor c) {
  sLedColor = c;
  
  /* 
   * According to https://www.3dbrew.org/wiki/MCURTC:SetInfoLEDPattern
   * animation pattern is as follow
   * u8 ??? | u8 loop_delay | u8 smoothing | u8 delay
   */
  RGBLedPattern pat;
  memset(&pat, 0, sizeof(pat));
  
  for(int i = 0; i < 32; i++) {
    pat.r[i] = sLedColor.red();
    pat.g[i] = sLedColor.green();
    pat.b[i] = sLedColor.blue();
  }
  
  pat.ani = 0x20;
  
  Ion::Simulator::CommonDriver::common_ptmsysmSetInfoLedPattern(pat);
}

void setBlinking(uint16_t period, float dutyCycle) {
  uint8_t period_3ds = (uint8_t)((float)(period)*0.016f);
  uint8_t duty_3ds = (uint8_t)(dutyCycle*32.0f);
  
  /* 
   * According to https://www.3dbrew.org/wiki/MCURTC:SetInfoLEDPattern
   * animation pattern is as follow
   * u8 ??? | u8 loop_delay | u8 smoothing | u8 delay
   *
   * Se, we set ??? to 0, loop_delay to 0 (to have it loop)
   */
  RGBLedPattern pat;
  memset(&pat, 0, sizeof(pat));
  
  for(int i = 0; i < duty_3ds && i < 32; i++) {
    pat.r[i] = (sLedColor.red() > 0) ? 255 : 0;
    pat.g[i] = (sLedColor.green() > 0) ? 255 : 0;
    pat.b[i] = (sLedColor.blue() > 0) ? 255 : 0;
  }
  
  pat.ani = period_3ds;
  
  Ion::Simulator::CommonDriver::common_ptmsysmSetInfoLedPattern(pat);
}

KDColor updateColorWithPlugAndCharge() {
  KDColor ledColor = getColor();
  if (ExamMode::FetchExamMode() == 0) { // If exam mode is on, we do not update the LED with the plugged/charging state
    if (USB::isPlugged()) {
      ledColor = Battery::isCharging() ? KDColorOrange : KDColorGreen;
    } else {
      ledColor = KDColorBlack;
    }
    setColor(ledColor);
  }
  return ledColor;
}

}
}
