#include <3ds.h>
#include <string.h>
#include "common.h"

static bool plugged = false;
static bool battery_charging = false;
static Ion::Battery::Charge battery_level = Ion::Battery::Charge::FULL;
static time_t last_pull = 0;

static Handle ptmsysmHandle = 0;

static Result common_ptmsysmInit() {
  return srvGetServiceHandle(&ptmsysmHandle, "ptm:sysm");
}

static Result common_ptmsysmExit() {
  return svcCloseHandle(ptmsysmHandle);
}

Result Ion::Simulator::CommonDriver::common_ptmsysmSetInfoLedPattern(RGBLedPattern pattern) {
  if (ptmsysmHandle == 0)
    return -1;

  u32* ipc = getThreadCommandBuffer();
  ipc[0] = 0x8010640;
  memcpy(&ipc[1], &pattern, 0x64);
  Result ret = svcSendSyncRequest(ptmsysmHandle);
  if(ret < 0) return ret;
  return ipc[1];
}

void Ion::Simulator::CommonDriver::init() {
  common_ptmsysmInit();
}

void Ion::Simulator::CommonDriver::deinit() {
  common_ptmsysmExit();
}

bool Ion::Simulator::CommonDriver::isPlugged() {
  pullData();
  return plugged;
}

bool Ion::Simulator::CommonDriver::isCharging() {
  pullData();
  return battery_charging;
}

Ion::Battery::Charge Ion::Simulator::CommonDriver::getLevel() {
  pullData();
  return battery_level;
}

void Ion::Simulator::CommonDriver::pullData() {
  time_t current = time(NULL);
  
  if (difftime(current, last_pull) >= PULL_DELAY) {
    PTMU_GetAdapterState(&plugged);
    
    u8 bat_level = 0;
    PTMU_GetBatteryLevel(&bat_level);
    
    switch(bat_level) {
      case 5:
        battery_level = Ion::Battery::Charge::FULL;
        break;
      case 4:
      case 3:
        battery_level = Ion::Battery::Charge::SOMEWHERE_INBETWEEN;
        break;
      case 2:
      case 1:
        battery_level = Ion::Battery::Charge::LOW;
        break;
      case 0:
      default:
        battery_level = Ion::Battery::Charge::EMPTY;
        break;
    }
    
    u8 bat_charging = 0;
    PTMU_GetBatteryChargeState(&bat_charging);
    battery_charging = (bool) bat_charging;
    last_pull = time(NULL);
  }
}

