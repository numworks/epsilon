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

bool TextFieldWithExtension::privateRemoveEndOfLine() {
  return removeTextBeforeExtension(false);
}

void TextFieldWithExtension::removeWholeText() {
  removeTextBeforeExtension(true);
  scrollToCursor();
}

bool TextFieldWithExtension::removeTextBeforeExtension(bool whole) {
  int extensionIndex = strlen(text()) - m_extensionLength;
  assert(extensionIndex >= 0 && extensionIndex < ContentView::k_maxBufferSize - m_extensionLength);
  int destinationIndex = whole ? 0 : cursorLocation();
  if (destinationIndex == extensionIndex) {
    return false;
  }
  assert(destinationIndex >= 0);
  assert(destinationIndex < extensionIndex);
  m_contentView.willModifyTextBuffer();
  strlcpy(&(m_contentView.textBuffer()[destinationIndex]), &(m_contentView.textBuffer()[extensionIndex]), ContentView::k_maxBufferSize);
  m_contentView.setCursorLocation(destinationIndex);
  m_contentView.didModifyTextBuffer();
  layoutSubviews();
  return true;
}

}
