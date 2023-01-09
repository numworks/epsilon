#ifndef APPS_CONTAINER_STORAGE_H
#define APPS_CONTAINER_STORAGE_H

#include "apps_container.h"
#include <omg/global_box.h>

#ifndef APPS_CONTAINER_SNAPSHOT_DECLARATIONS
#error Missing snapshot declarations
#endif

class AppsContainerStorage : public AppsContainer {
public:
  AppsContainerStorage();
  static OMG::GlobalBox<AppsContainerStorage> sharedAppsContainerStorage;
  int numberOfBuiltinApps() override;
  Escher::App::Snapshot * appSnapshotAtIndex(int index) override;
  void * currentAppBuffer() override;
private:
  APPS_CONTAINER_SNAPSHOT_DECLARATIONS
};

#endif
