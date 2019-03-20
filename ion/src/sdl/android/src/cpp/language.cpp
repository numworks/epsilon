#include "../../../shared/platform.h"

const char * IonSDLPlatformGetLanguageCode() {
  // The Android NDK cannot retrieve the current locale
  // This will be handled by the Java side
  return nullptr;
}
