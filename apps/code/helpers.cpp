#include "helpers.h"
#include "clipboard.h"
#include <string.h>

using namespace Escher;

namespace Code {
namespace Helpers {

const char * PythonTextForEvent(Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    return "x";
  }
  if (event.text() == nullptr) {
    return nullptr;
  }
  for (size_t i=0; i<Clipboard::k_numberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = Clipboard::PythonTextPairs()[i];
    if (strcmp(event.text(), pair.firstString()) == 0) {
      return pair.secondString();
    }
  }
  return nullptr;
}

}
}
