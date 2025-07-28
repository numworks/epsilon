#include <winnls.h>

#include "../shared/platform.h"

namespace Ion {
namespace Simulator {
namespace Platform {

const char* languageCode() {
  /* Per documentation, the maximum number of characters allowed for the
   * language string is nine, including a terminating null character. */
  static char buffer[9] = {0};
  if (buffer[0] == 0) {
    int result = GetLocaleInfoA(
        LOCALE_USER_DEFAULT,     // Retrieve default user locale
        LOCALE_SISO639LANGNAME,  // And more specifically, it's two-letter lang
        buffer, 9);
    if (result == 0) {
      return nullptr;
    }
  }
  return buffer;
}

}  // namespace Platform
}  // namespace Simulator
}  // namespace Ion
