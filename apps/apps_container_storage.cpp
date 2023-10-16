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

int AppsContainerStorage::numberOfApps() {
  return k_numberOfCommonApps;
}

App::Snapshot * AppsContainerStorage::appSnapshotAtIndex(int index) {
  if (index < 0) {
    return nullptr;
  }
  App::Snapshot * snapshots[] = {
    homeAppSnapshot()
    APPS_CONTAINER_SNAPSHOT_LIST
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == k_numberOfCommonApps);
  assert(index >= 0 && index < k_numberOfCommonApps);
  // To avoid crashes, we return the home app snapshot if the index is out of
  // bounds. (no crash in release mode, but an assert in debug mode)
  if (index >= k_numberOfCommonApps) {
    return snapshots[0];
  }
  return snapshots[index];
}

// Get the index of the app from its snapshot
int AppsContainerStorage::appIndexFromSnapshot(App::Snapshot * snapshot) {
  App::Snapshot * snapshots[] = {
    homeAppSnapshot()
    APPS_CONTAINER_SNAPSHOT_LIST
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == k_numberOfCommonApps);
  for (int i = 0; i < k_numberOfCommonApps; i++) {
    if (snapshots[i] == snapshot) {
      return i;
    }
  }
  return NULL;
}
