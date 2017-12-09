#ifndef APPS_CONTAINER_STORAGE_H
#define APPS_CONTAINER_STORAGE_H

#include "apps_container.h"

#ifndef APPS_CONTAINER_SNAPSHOT_DECLARATIONS
#error Missing snapshot declarations
#endif

class AppsContainerStorage final : public AppsContainer {
public:
  static AppsContainerStorage * sharedContainer();
  AppsContainerStorage();
  int numberOfApps() override;
  App::Snapshot * appSnapshotAtIndex(int index) override;
private:
  APPS_CONTAINER_SNAPSHOT_DECLARATIONS
};

#endif
