#ifndef APPS_CONTAINER_HELPER_H
#define APPS_CONTAINER_HELPER_H

/* This intermediary class allow the unit tests to catch and handle global
 * context calls, as well as calling some methods even though it has no apps
 * container. */
class AppsContainerHelper {
 public:
  static void notifyCountryChangeToSnapshots();
};

#endif
