#include "helpers.h"
#include <escher/clipboard.h>
#include <apps/apps_container.h>

namespace Code {
namespace Helpers {

bool PythonTextForEvent(Ion::Events::Event event, char * buffer, bool * shouldRemoveLastCharacter) {
  for (size_t i=0; i<NumberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = PythonTextPairs[i];
    if (event.text() == pair.firstString()) {
      strcpy(buffer, pair.secondString());
      return true;
    }
    if (event == Ion::Events::XNT) {
      CodePoint XNT = AppsContainer::sharedAppsContainer()->XNT('x', shouldRemoveLastCharacter);
      buffer[UTF8Decoder::CodePointToChars(XNT, buffer, CodePoint::MaxCodePointCharLength + 1)] = 0;
      return true;
    }
    return false;
  }
}
}
}
