#include <ion/clock.h>
#include "regs/regs.h"

// Public Ion methods

namespace Ion {
namespace Clock {

static void rtcSetWriteEnable(bool status) {
  if (status) {
    PWR.CR()->setDBP(true);
    RTC.WPR()->setKEY(0xCA);
    RTC.WPR()->setKEY(0x53);
  }
  else {
    RTC.WPR()->setKEY(0xFF);
    PWR.CR()->setDBP(false);
  }
}

static void rtcSetInitializationMode(bool status) {
  if (status) {
    RTC.ISR()->setINIT(true);
    while (RTC.ISR()->getINITF() != true);
  }
  else {
    RTC.ISR()->setINIT(false);
  }
}

static void rtcWaitSync() {
  RTC.ISR()->setRSF(false);
  while (RTC.ISR()->getRSF() == false);
}

void clock(int *hours, int *mins) {
  int t_hours, t_mins;

  Ion::Clock::rtcWaitSync();

  t_hours = RTC.TR()->getHT() * 10 + RTC.TR()->getHU();
  t_mins = RTC.TR()->getMNT() * 10 + RTC.TR()->getMNU();

  if (hours)
    *hours = t_hours;
  if (mins)
    *mins = t_mins;
}

void setClock(int hours, int mins) {
  rtcSetWriteEnable(true);
  rtcSetInitializationMode(true);

  RTC.PRER()->setPREDIV_S(0xF9);
  RTC.PRER()->setPREDIV_A(0x7F);
  RTC.CR()->setFMT(false);
  
  class RTC::TR tr(0);
  tr.setPM(false);
  tr.setHT(hours / 10);
  tr.setHU(hours % 10);
  tr.setMNT(mins / 10);
  tr.setMNU(mins % 10);
  tr.setST(0);
  tr.setSU(0);
  RTC.TR()->set(tr);

  class RTC::DR dr(0);
  dr.setYT(0);
  dr.setYU(0);
  dr.setWDU(1);
  dr.setMT(0);
  dr.setMU(1);
  dr.setDT(0);
  dr.setDU(1);
  RTC.DR()->set(dr);

  rtcSetInitializationMode(false);
  rtcSetWriteEnable(false);
}

}
}

// Private Ion::Device::RTC methods

namespace Ion {
namespace RTC {
namespace Device {

void init() {
  PWR.CR()->setDBP(true);
  // Enable LSI clock
  RCC.CSR()->setLSION(true);
  while (RCC.CSR()->getLSIRDY() != true);
  // Initialize backup domain for RTC
  RCC.BDCR()->setRTCSEL(0x2);
  RCC.BDCR()->setRTCEN(true);
  PWR.CR()->setDBP(false);

  Ion::Clock::rtcWaitSync();
  Ion::Clock::setClock(0, 0);
}

}
}
}
