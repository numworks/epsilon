#include "helpers.h"
#include <escher/clipboard.h>

namespace Code {
namespace Helpers {

const char * PythonTextForEvent(Ion::Events::Event event) {
  for (size_t i=0; i<NumberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = PythonTextPairs[i];
    if (event.text() == pair.firstString()) {
      return pair.secondString();
    }
  }
  return nullptr;
}
}
}
