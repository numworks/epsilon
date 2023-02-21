#ifndef APPS_CONTAINER_HELPER_H
#define APPS_CONTAINER_HELPER_H

#include <apps/shared/global_context.h>

/* This intermediary class allow the unit tests to catch and handle global
 * context calls, even though it has no apps container. */
class AppsContainerHelper {
 public:
  static Shared::GlobalContext* sharedAppsContainerGlobalContext();
};

#endif
