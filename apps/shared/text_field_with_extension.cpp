#include "text_field_with_extension.h"

namespace Shared {

void TextFieldWithExtension::willSetCursorTextLocation(const char * * location) {
  size_t textLength = strlen(text());
  assert(textLength >= m_extensionLength);
  const char * maxLocation = m_contentView.draftTextBuffer() + (textLength - m_extensionLength);
  if (*location > maxLocation) {
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
  assert(isEditing());
  const char * extension = m_contentView.draftTextBuffer() + (strlen(text()) - m_extensionLength);
  assert(extension >= m_contentView.draftTextBuffer() && extension < m_contentView.draftTextBuffer() + (ContentView::k_maxBufferSize - m_extensionLength));
  char * destination = whole ? m_contentView.draftTextBuffer() : const_cast<char *>(cursorTextLocation());
  if (destination == extension) {
    return false;
  }
  assert(destination >= m_contentView.draftTextBuffer());
  assert(destination < extension);
  m_contentView.willModifyTextBuffer();
  strlcpy(destination, extension, ContentView::k_maxBufferSize - (destination - m_contentView.draftTextBuffer()));
  m_contentView.setCursorTextLocation(destination);
  m_contentView.didModifyTextBuffer();
  layoutSubviews();
  return true;
}

}
