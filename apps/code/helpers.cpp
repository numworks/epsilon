#include "helpers.h"
#include <escher/clipboard.h>
#include <ion/events.h>
#include <layout_events.h>

using namespace Escher;
namespace Code {
namespace Helpers {

const char * PythonTextForEvent(Ion::Events::Event event) {
  for (size_t i=0; i<NumberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = PythonTextPairs[i];
    char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
    event.copyText(buffer, Ion::Events::EventData::k_maxDataSize);
    if (strcmp(buffer, pair.firstString()) == 0) {
      return pair.secondString();
    }
    if (event == Ion::Events::XNT) {
      return "x";
    }
  }
  return nullptr;
}
}
}
