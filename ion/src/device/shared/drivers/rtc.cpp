#include <ion/rtc.h>
#include <ion/timing.h>
#include <drivers/rtc.h>
#include <drivers/config/clocks.h>
#include "regs/regs.h"

constexpr int yearEpoch = 2000;

static int bcdToBinary(int tens, int units) {
  return tens * 10 + units;
}

static int dayOfWeek(int y, int m, int d)
{
    const static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return ((y + y/4 - y/100 + y/400 + t[m-1] + (d-1)) % 7) + 1;
}

// Public Ion methods

namespace Ion {
namespace RTC {

static void rtcSetWriteEnable(bool status) {
  if (status) {
    Device::Regs::PWR.CR()->setDBP(true);
    Device::Regs::RTC.WPR()->setKEY(0xCA);
    Device::Regs::RTC.WPR()->setKEY(0x53);
  }
  else {
    Device::Regs::RTC.WPR()->setKEY(0xFF);
    Device::Regs::PWR.CR()->setDBP(false);
  }
}

void setMode(Mode mode) {
  DateTime prevDateTime = dateTime();
  Ion::Device::RTC::init(mode != Mode::Disabled, mode == Mode::HSE);
  setDateTime(prevDateTime);
}

Mode mode() {
  if (!Ion::Device::Regs::RCC.BDCR()->getRTCEN()) {
    return Mode::Disabled;
  }
  if (Ion::Device::Regs::RCC.BDCR()->getRTCSEL() == 0x3) {
    return Mode::HSE;
  }
  return Mode::LSI;
}

DateTime dateTime() {
  if (mode() == Mode::Disabled) {
    return DateTime { 0, 0, 0, 1, 1, 2000, 6 };
  }

  Device::Regs::RTC.ISR()->setRSF(false);
  for (int cpt = 0; cpt < 100; cpt++) {
    if (Device::Regs::RTC.ISR()->getRSF() == true)
      break;
    Ion::Timing::usleep(5);
  }

  return DateTime {
    bcdToBinary(Device::Regs::RTC.TR()->getST(),  Device::Regs::RTC.TR()->getSU()),
    bcdToBinary(Device::Regs::RTC.TR()->getMNT(), Device::Regs::RTC.TR()->getMNU()),
    bcdToBinary(Device::Regs::RTC.TR()->getHT(),  Device::Regs::RTC.TR()->getHU()),
    bcdToBinary(Device::Regs::RTC.DR()->getDT(),  Device::Regs::RTC.DR()->getDU()),
    bcdToBinary(Device::Regs::RTC.DR()->getMT(),  Device::Regs::RTC.DR()->getMU()),
    bcdToBinary(Device::Regs::RTC.DR()->getYT(),  Device::Regs::RTC.DR()->getYU()) + yearEpoch,
    Device::Regs::RTC.DR()->getWDU() - 1,
  };
}

void setDateTime(DateTime dateTime) {
  if (mode() == Mode::Disabled) {
    return;
  }

  rtcSetWriteEnable(true);

  Device::Regs::RTC.ISR()->setINIT(true);
  for (int cpt = 0; cpt < 100; cpt++) {
    if (Device::Regs::RTC.ISR()->getINITF() == true)
      break;
    Ion::Timing::usleep(10);
  }

  if (Ion::Device::Regs::RCC.BDCR()->getRTCSEL() == 0x2) {
    // LSI is ~32 kHz
    Device::Regs::RTC.PRER()->setPREDIV_S(249);
    Device::Regs::RTC.PRER()->setPREDIV_A(127);
  }
  else {
    // HSE's divided down to 1 MHz
    Device::Regs::RTC.PRER()->setPREDIV_S(7999);
    Device::Regs::RTC.PRER()->setPREDIV_A(124);
  }
  Device::Regs::RTC.CR()->setFMT(false);
  
  class Device::Regs::RTC::TR tr(0);
  tr.setPM(false);
  tr.setHT(dateTime.tm_hour / 10);
  tr.setHU(dateTime.tm_hour % 10);
  tr.setMNT(dateTime.tm_min / 10);
  tr.setMNU(dateTime.tm_min % 10);
  tr.setST(dateTime.tm_sec / 10);
  tr.setSU(dateTime.tm_sec % 10);
  Device::Regs::RTC.TR()->set(tr);

  class Device::Regs::RTC::DR dr(0);
  dr.setYT((dateTime.tm_year - yearEpoch) / 10);
  dr.setYU((dateTime.tm_year - yearEpoch) % 10);
  dr.setWDU(dayOfWeek(dateTime.tm_year, dateTime.tm_mon, dateTime.tm_mday));
  dr.setMT(dateTime.tm_mon / 10);
  dr.setMU(dateTime.tm_mon % 10);
  dr.setDT(dateTime.tm_mday / 10);
  dr.setDU(dateTime.tm_mday % 10);
  Device::Regs::RTC.DR()->set(dr);

  Device::Regs::RTC.ISR()->setINIT(true);
  rtcSetWriteEnable(false);
}

}
}

// Private Ion::Device::RTC methods

namespace Ion {
namespace Device {
namespace RTC {

void init(bool enable, bool useHighPrecisionClock) {
  const int rtcSource = useHighPrecisionClock ? 0x3 : 0x2;

  Ion::Device::Regs::RCC.CFGR()->setRTCPRE(Ion::Device::Clocks::Config::HSE);

  Ion::RTC::rtcSetWriteEnable(true);

  if (Ion::Device::Regs::RCC.BDCR()->getRTCSEL() != rtcSource) {
    // Initialize backup domain for RTC
    Ion::Device::Regs::RCC.BDCR()->setBDRST(true);
    Ion::Timing::usleep(250);
    Ion::Device::Regs::RCC.BDCR()->setBDRST(false);
    Ion::Timing::usleep(250);

    Ion::RTC::rtcSetWriteEnable(true);
  }

  Ion::Device::Regs::RCC.BDCR()->setRTCSEL(rtcSource);
  Ion::Device::Regs::RCC.BDCR()->setRTCEN(enable);

  // Enable/disable LSI clock
  if (enable && useHighPrecisionClock) {
    Ion::Device::Regs::RCC.CSR()->setLSION(false);  
  }
  else {
    Ion::Device::Regs::RCC.CSR()->setLSION(true);
    while (Ion::Device::Regs::RCC.CSR()->getLSIRDY() != true);
  }

  Ion::RTC::rtcSetWriteEnable(false);
}

}
}
}
