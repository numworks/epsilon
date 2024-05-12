#include "../shared/platform.h"
#include <locale.h>

namespace Ion {
namespace Simulator {
namespace Platform {

const char * languageCode() {
  static char buffer[3] = {0};
  char * locale = setlocale(LC_ALL, "");
  if (locale && locale[2] == '_') { // Check if locale is not null before accessing its elements
    buffer[0] = locale[0];
    buffer[1] = locale[1];
    return buffer;
  }
  return nullptr;
}

}
}
}
