#include <escher/text_input.h>
#include <assert.h>

/* TextInput::ContentView */

TextInput::ContentView::ContentView(KDText::FontSize size, KDColor textColor, KDColor backgroundColor) :
  View(),
  m_cursorView(),
  m_fontSize(size),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_cursorIndex(0)
{
}

void TextInput::ContentView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextInput::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextInput::ContentView::setCursorLocation(int location) {
  int adjustedLocation = location < 0 ? 0 : location;
  adjustedLocation = adjustedLocation > (signed int)editedTextLength() ? (signed int)editedTextLength() : adjustedLocation;
  m_cursorIndex = adjustedLocation;
  layoutSubviews();
}

KDRect TextInput::ContentView::cursorRect() {
  return characterFrameAtIndex(m_cursorIndex);
}

int TextInput::ContentView::numberOfSubviews() const {
  return 1;
}

View * TextInput::ContentView::subviewAtIndex(int index) {
  return &m_cursorView;
}

void TextInput::ContentView::layoutSubviews() {
  m_cursorView.setFrame(cursorRect());
}

void TextInput::ContentView::reloadRectFromCursorPosition(size_t index, bool lineBreak) {
  KDRect charRect = characterFrameAtIndex(index);
  KDRect dirtyRect = KDRect(charRect.x(), charRect.y(), bounds().width() - charRect.x(), charRect.height());
  if (lineBreak) {
      dirtyRect = dirtyRect.unionedWith(KDRect(0, charRect.bottom()+1, bounds().width(), bounds().height()-charRect.bottom()-1));
  }
  markRectAsDirty(dirtyRect);
}

/* TextInput */

TextInput::TextInput(Responder * parentResponder, View * contentView) :
  ScrollableView(parentResponder, contentView, this)
{
}

Toolbox * TextInput::toolbox() {
  if (delegate()) {
    return delegate()->toolboxForTextInput(this);
  }
  return nullptr;
}

void TextInput::setBackgroundColor(KDColor backgroundColor) {
  contentView()->setBackgroundColor(backgroundColor);
}

void TextInput::setTextColor(KDColor textColor) {
  contentView()->setTextColor(textColor);
}

bool TextInput::removeChar() {
  contentView()->removeChar();
  scrollToCursor();
  return true;
}

void TextInput::scrollToCursor() {
  /* Technically, we do not need to overscroll in text input. However,
   * logically, we should layout the scroll view before calling
   * scrollToContentRect in case the size of the scroll view has changed and
   * then call scrollToContentRect which call another layout of the scroll view
   * if the offset has evolved. In order to avoid requiring two layouts, we
   * allow overscrolling in scrollToContentRect and the last layout of the
   * scroll view corrects the size of the scroll view only once. */
  scrollToContentRect(contentView()->cursorRect(), true);
}

bool TextInput::setCursorLocation(int location) {
  contentView()->setCursorLocation(location);
  scrollToCursor();
  return true;
}

bool TextInput::insertTextAtLocation(const char * text, int location) {
  if (contentView()->insertTextAtLocation(text, location)) {
    /* We layout the scrollable view before scrolling to cursor because the
     *  content size might have changed. */
    layoutSubviews();
    scrollToCursor();
    return true;
  }
  return false;
}

bool TextInput::removeEndOfLine() {
  if (contentView()->removeEndOfLine()) {
    scrollToCursor();
    return true;
  }
  return false;
}
