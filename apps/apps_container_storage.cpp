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

// Take the Home app into account
constexpr int k_numberOfCommonApps = 1+APPS_CONTAINER_SNAPSHOT_COUNT;

OMG::GlobalBox<AppsContainerStorage> AppsContainerStorage::sharedAppsContainerStorage;

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
  assert(index < numberOfBuiltinApps());
  Escher::App::Snapshot * snapshots[] = {
    homeAppSnapshot()
    APPS_CONTAINER_SNAPSHOT_LIST
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == numberOfBuiltinApps());
  assert(index >= 0 && index < numberOfBuiltinApps());
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
  /* We use the $ char to be able to link this symbol at a specific location,
   * namely, the end of bss section, in order to extend the external app
   * sandbox range. */
  static Apps s_apps
#if PLATFORM_DEVICE
    __attribute__((section(".bss.$app_buffer")));
#else
  ;
#endif
  return &s_apps;
}
