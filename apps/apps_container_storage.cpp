#include "apps_container_storage.h"

#ifndef APPS_CONTAINER_SNAPSHOT_CONSTRUCTORS
#error Missing snapshot constructors
#endif

#ifndef APPS_CONTAINER_SNAPSHOT_LIST
#error Missing snapshot list
#endif

#ifndef APPS_CONTAINER_SNAPSHOT_COUNT
#error Missing snapshot count
#endif

constexpr int k_numberOfCommonApps = 1+APPS_CONTAINER_SNAPSHOT_COUNT; // Take the Home app into account

AppsContainerStorage::AppsContainerStorage() :
  AppsContainer()
  APPS_CONTAINER_SNAPSHOT_CONSTRUCTORS
{
}

int AppsContainerStorage::numberOfBuiltinApps() {
  return k_numberOfCommonApps;
}

Escher::App::Snapshot * AppsContainerStorage::appSnapshotAtIndex(int index) {
  if (index < 0) {
    return nullptr;
  }
  assert(index < k_numberOfCommonApps);
  Escher::App::Snapshot * snapshots[] = {
    homeAppSnapshot()
    APPS_CONTAINER_SNAPSHOT_LIST
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == k_numberOfCommonApps);
  assert(index >= 0 && index < k_numberOfCommonApps);
  return snapshots[index];
}

union Apps {
public:
  /* Enforce a trivial constructor and destructor that just leave the memory
   * unmodified. This way, m_apps can be trivially destructed. */
  Apps() {};
  ~Apps() {};
private:
  APPS_CONTAINER_APPS_DECLARATION
    Home::App m_homeApp;
  OnBoarding::App m_onBoardingApp;
  HardwareTest::App m_hardwareTestApp;
  USB::App m_usbApp;
};


void * AppsContainerStorage::currentAppBuffer() {
  static Apps s_apps __attribute__((section(".app_buffer")));
  return &s_apps;
}
