#include "warning_display.h"
#include "display_helper.h"
#include <drivers/display.h>
#include <ion/display.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace WarningDisplay {

void showMessage(const char * const * messages, int numberOfMessages) {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Device::Display::pushRectUniform(screen, KDColorWhite);
  // TODO EMILIE: Would it be more optimized to directly use an image of the warning?
  const char * title = messages[0];
  KDCoordinate currentHeight = 60;
  DisplayHelper::drawString(title, &currentHeight, KDFont::LargeFont);
  currentHeight += KDFont::LargeFont->glyphSize().height();
  for (int j = 1; j < numberOfMessages; j++) {
    const char * message = messages[j];
    DisplayHelper::drawString(message, &currentHeight, KDFont::SmallFont);
  }
}

}
}
}
