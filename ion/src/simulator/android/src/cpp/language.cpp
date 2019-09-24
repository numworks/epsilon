#include "../../../shared/platform.h"

char * IonSDLPlatformGetLanguageCode() {
  // The Android NDK cannot retrieve the current locale
  // This will be handled by the Java side
  return nullptr;
}
