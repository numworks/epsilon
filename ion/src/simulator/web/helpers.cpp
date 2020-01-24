#include <ion.h>
#include "../shared/main.h"

extern "C" const char * IonSoftwareVersion() {
  return Ion::softwareVersion();
}

extern "C" const char * IonPatchLevel() {
  return Ion::patchLevel();
}

extern "C" void IonDisplayForceRefresh() {
  Ion::Simulator::Main::setNeedsRefresh();
  Ion::Simulator::Main::refresh();
}
