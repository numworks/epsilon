#include <escher/text_input.h>
#include <assert.h>

/* TextInput::ContentView */

void TextInput::ContentView::setCursorLocation(int location) {
  assert(location >= 0);
  int adjustedLocation = location > (signed int)editedTextLength() ? (signed int)editedTextLength() : location;
  m_cursorIndex = adjustedLocation;
  layoutSubviews();
}

void TextInput::ContentView::setFont(const KDFont * font) {
  m_font = font;
  markRectAsDirty(bounds());
}

KDRect TextInput::ContentView::cursorRect() {
  return characterFrameAtIndex(m_cursorIndex);
}

void TextInput::ContentView::layoutSubviews() {
  m_cursorView.setFrame(cursorRect());
}

KDRect TextInput::ContentView::dirtyRectFromCursorPosition(size_t index, bool lineBreak) const {
  KDRect charRect = characterFrameAtIndex(index);
  KDRect dirtyRect = KDRect(charRect.x(), charRect.y(), bounds().width() - charRect.x(), charRect.height());
  if (lineBreak) {
    dirtyRect = dirtyRect.unionedWith(KDRect(0, charRect.bottom()+1, bounds().width(), bounds().height()-charRect.bottom()-1));
  }
  return dirtyRect;
}

void TextInput::ContentView::reloadRectFromCursorPosition(size_t index, bool lineBreak) {
  markRectAsDirty(dirtyRectFromCursorPosition(index, lineBreak));
}

/* TextInput */

bool TextInput::removeChar() {
  contentView()->removeChar();
  scrollToCursor();
  return true;
}

void TextInput::scrollToCursor() {
  /* Technically, we do not need to overscroll in text input. However, we should
   * layout the scroll view before calling scrollToContentRect (in case the size
   * of the scroll view has changed) and then call scrollToContentRect which
   * calls another layout of the scroll view if the offset has evolved.
   *
   * In order to avoid requiring two layouts, we allow overscrolling in
   * scrollToContentRect, and the last layout of the scroll view corrects the
   * size of the scroll view only once. */
  scrollToContentRect(contentView()->cursorRect(), true);
}

bool TextInput::setCursorLocation(int location) {
  int adjustedLocation = location < 0 ? 0 : location;
  willSetCursorLocation(&adjustedLocation);
  contentView()->setCursorLocation(adjustedLocation);
  scrollToCursor();
  return true;
}

bool TextInput::insertTextAtLocation(const char * text, int location) {
  if (contentView()->insertTextAtLocation(text, location)) {
    /* We layout the scrollable view before scrolling to cursor because the
     * content size might have changed. */
    layoutSubviews();
    scrollToCursor();
    return true;
  }
  return false;
}

bool TextInput::removeEndOfLine() {
  if (privateRemoveEndOfLine()) {
    scrollToCursor();
    return true;
  }
  return false;
}

bool TextInput::privateRemoveEndOfLine() {
  return contentView()->removeEndOfLine();
}
