#include "text_field_with_extension.h"

namespace Shared {

void TextFieldWithExtension::willSetCursorLocation(int * location) {
  size_t textLength = strlen(text());
  assert(textLength >= m_extensionLength);
  size_t maxLocation = textLength - m_extensionLength;
  if (*location > (int)maxLocation) {
    *location = maxLocation;
  }
}

}
