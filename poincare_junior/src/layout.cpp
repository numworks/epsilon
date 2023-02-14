#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference Layout::EditionPoolTextToLayout(const char * text) {
  int n = std::strlen(text);
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(n);
  for (int i = 0; i < n; i++) {
    EditionReference::Push<BlockType::CodePointLayout, CodePoint>(text[i]);
  }
  return ref;
}

Layout Layout::Parse(const char * textInput) {
  return Layout([](const char * text) {
      EditionPoolTextToLayout(text);
    }, textInput);
}

void Layout::toText(char * buffer, size_t bufferSize) const {
  memcpy(buffer, "-1+2*3", bufferSize);
}

}
