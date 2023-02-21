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
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  Ion::Events::copyText(static_cast<uint8_t>(event), buffer,
                        Ion::Events::EventData::k_maxDataSize);
  if (buffer[0] == 0) {
    return nullptr;
  }
  for (size_t i = 0; i < Clipboard::k_numberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = Clipboard::PythonTextPairs()[i];
    if (strcmp(buffer, pair.firstString()) == 0) {
      return pair.secondString();
    }
  }
  return nullptr;
}

}  // namespace Helpers
}  // namespace Code
