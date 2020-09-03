#include <ion.h>
#include "../shared/window.h"

extern "C" const char * IonSoftwareVersion() {
  return Ion::softwareVersion();
}

extern "C" const char * IonPatchLevel() {
  return Ion::patchLevel();
}

extern "C" void IonDisplayForceRefresh() {
  Ion::Simulator::Window::setNeedsRefresh();
  Ion::Simulator::Window::refresh();
}
