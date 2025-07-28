#include <locale.h>

#include "../shared/platform.h"

namespace Ion {
namespace Simulator {
namespace Platform {

const char* languageCode() {
  static char buffer[3] = {0};
  char* locale = setlocale(LC_ALL, "");
  if (locale[2] == '_') {
    buffer[0] = locale[0];
    buffer[1] = locale[1];
    return buffer;
  }
  return nullptr;
}

}  // namespace Platform
}  // namespace Simulator
}  // namespace Ion
