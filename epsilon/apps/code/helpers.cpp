#include "helpers.h"

#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <string.h>

#include "clipboard.h"

using namespace Escher;
namespace Code {
namespace Helpers {

const char* PythonTextForEvent(Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    return "x";
  }
  if (event == Ion::Events::Imaginary) {
    return "1j";
  }
  const char* eventText = event.text();
  if (eventText == nullptr || eventText[0] == 0) {
    return nullptr;
  }
  for (size_t i = 0; i < Clipboard::k_numberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = Clipboard::PythonTextPairs()[i];
    if (strcmp(eventText, pair.firstString()) == 0) {
      return pair.secondString();
    }
  }
  return nullptr;
}

}  // namespace Helpers
}  // namespace Code
