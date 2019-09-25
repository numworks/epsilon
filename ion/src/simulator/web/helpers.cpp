#include <ion.h>

extern "C" const char * IonSoftwareVersion() {
  return Ion::softwareVersion();
}

extern "C" const char * IonPatchLevel() {
  return Ion::patchLevel();
}
