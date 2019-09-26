#include "../shared/platform.h"

#include <locale.h>

char * IonSimulatorGetLanguageCode() {
  static char buffer[3] = {0};
  char * locale = setlocale(LC_ALL, "");
  if (locale[2] == '_') {
    buffer[0] = locale[0];
    buffer[1] = locale[1];
    return buffer;
  }
  return nullptr;
}
