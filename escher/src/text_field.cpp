#include <escher/text_field.h>
#include <escher/text_input_helpers.h>
#include <escher/clipboard.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

static inline int minInt(int x, int y) { return x < y ? x : y; }
static char s_draftTextBuffer[TextField::maxBufferSize()];

/* TextField::ContentView */

TextField::ContentView::ContentView(char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize, const KDFont * font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput::ContentView(font),
  m_isEditing(false),
  m_textBuffer(textBuffer),
  m_textBufferSize(textBufferSize),
  m_draftTextBufferSize(draftTextBufferSize),
  m_currentDraftTextLength(0),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
  if (textBuffer == nullptr) {
    m_textBuffer = s_draftTextBuffer;
  }
  assert(m_draftTextBufferSize <= k_maxBufferSize);
  assert(m_textBufferSize <= k_maxBufferSize);
  reinitDraftTextBuffer();
}

void TextField::ContentView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextField::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextField::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backgroundColor = m_backgroundColor;
  if (m_isEditing) {
    backgroundColor = KDColorWhite;
  }
  ctx->fillRect(bounds(), backgroundColor);
  ctx->drawString(text(), glyphFrameAtPosition(text(), text()).origin(), m_font, m_textColor, backgroundColor);
}

const char * TextField::ContentView::text() const {
  return const_cast<const char *>(m_isEditing ? s_draftTextBuffer : m_textBuffer);
}

const char * TextField::ContentView::editedText() const {
  return s_draftTextBuffer;
}

void TextField::ContentView::setText(const char * text) {
  size_t textRealLength = strlen(text);
  size_t maxBufferSize = m_textBufferSize;
  char * buffer = m_textBuffer;
  if (m_isEditing) {
    maxBufferSize = m_draftTextBufferSize;
    buffer = s_draftTextBuffer;
  }
  int textLength = minInt(textRealLength, maxBufferSize - 1);
  // Copy the text
  strlcpy(buffer, text, maxBufferSize);
  // Update the draft text length
  if (m_isEditing || m_textBuffer == s_draftTextBuffer) {
    m_currentDraftTextLength = textLength;
  }
  markRectAsDirty(bounds());
}

void TextField::ContentView::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

void TextField::ContentView::setEditing(bool isEditing) {
  if (m_isEditing == isEditing) {
    return;
  }
  m_isEditing = isEditing;
  m_currentDraftTextLength = strlen(s_draftTextBuffer);
  if (m_cursorLocation < s_draftTextBuffer
      || m_cursorLocation > s_draftTextBuffer + m_currentDraftTextLength)
  {
    m_cursorLocation = s_draftTextBuffer + m_currentDraftTextLength;
  }
  markRectAsDirty(bounds());
  layoutSubviews();
}

void TextField::ContentView::reinitDraftTextBuffer() {
  /* We first need to clear the buffer, otherwise setCursorLocation might do
   * various operations on a buffer with maybe non-initialized content, such as
   * stringSize, etc. Those operation might be perilous on non-UTF8 content. */
  s_draftTextBuffer[0] = 0;
  m_currentDraftTextLength = 0;
  setCursorLocation(s_draftTextBuffer);
}

bool TextField::ContentView::insertTextAtLocation(const char * text, const char * location) {
  assert(m_isEditing);

  int textSize = strlen(text);
  if (m_currentDraftTextLength + textSize >= m_draftTextBufferSize || textSize == 0) {
    return false;
  }

  memmove(const_cast<char *>(location + textSize), location, (s_draftTextBuffer + m_currentDraftTextLength + 1) - location);

  // Caution! One byte will be overridden by the null-terminating char of strlcpy
  char * overridenByteLocation = const_cast<char *>(location + strlen(text));
  char overridenByte = *overridenByteLocation;
  strlcpy(const_cast<char *>(location), text, (s_draftTextBuffer + m_draftTextBufferSize) - location);
  assert(overridenByteLocation < s_draftTextBuffer + m_draftTextBufferSize);
  *overridenByteLocation = overridenByte;
  m_currentDraftTextLength += textSize;

  UTF8Decoder decoder(s_draftTextBuffer);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  assert(!codePoint.isCombining());
  while (codePoint != UCodePointNull) {
    assert(codePointPointer < s_draftTextBuffer + m_draftTextBufferSize);
    if (codePoint == '\n') {
      assert(UTF8Decoder::CharSizeOfCodePoint('\n') == 1);
      *(const_cast<char *>(codePointPointer)) = 0;
      m_currentDraftTextLength = codePointPointer - s_draftTextBuffer;
      break;
    }
    codePointPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }

  reloadRectFromPosition(m_horizontalAlignment == 0.0f ? location : s_draftTextBuffer);
  return true;
}

KDSize TextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize stringSize = m_font->stringSize(text());
  assert(stringSize.height() == m_font->glyphSize().height());
  if (m_isEditing) {
    return KDSize(stringSize.width() + m_cursorView.minimalSizeForOptimalDisplay().width(), stringSize.height());
  }
  return stringSize;
}

bool TextField::ContentView::removePreviousGlyph() {
  assert(m_isEditing);

  if (m_horizontalAlignment > 0.0f) {
    /* Reload the view. If we do it later, the text beins supposedly shorter, we
     * will not clean the first char. */
    reloadRectFromPosition(s_draftTextBuffer);
  }
  // Remove the glyph if possible
  int removedSize = UTF8Helper::RemovePreviousGlyph(s_draftTextBuffer, const_cast<char *>(cursorLocation()));
  if (removedSize == 0) {
    assert(cursorLocation() == s_draftTextBuffer);
    return false;
  }

  // Update the draft buffer length
  m_currentDraftTextLength-= removedSize;
  assert(s_draftTextBuffer[m_currentDraftTextLength] == 0);

  // Set the cursor location and reload the view
  assert(cursorLocation() - removedSize >= s_draftTextBuffer);
  setCursorLocation(cursorLocation() - removedSize);
  if (m_horizontalAlignment == 0.0f) {
    reloadRectFromPosition(cursorLocation());
  }
  layoutSubviews();
  return true;
}

bool TextField::ContentView::removeEndOfLine() {
  assert(m_isEditing);
  size_t lengthToCursor = (size_t)(cursorLocation() - s_draftTextBuffer);
  if (m_currentDraftTextLength == lengthToCursor) {
    return false;
  }
  reloadRectFromPosition(m_horizontalAlignment == 0.0f ? cursorLocation() : s_draftTextBuffer);
  m_currentDraftTextLength = lengthToCursor;
  *(const_cast<char *>(cursorLocation())) = 0;
  layoutSubviews();
  return true;
}

void TextField::ContentView::willModifyTextBuffer() {
  assert(m_isEditing);
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  reloadRectFromPosition(s_draftTextBuffer);
}

void TextField::ContentView::didModifyTextBuffer() {
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  m_currentDraftTextLength = strlen(s_draftTextBuffer);
  layoutSubviews();
}

void TextField::ContentView::layoutSubviews() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  TextInput::ContentView::layoutSubviews();
}

KDRect TextField::ContentView::glyphFrameAtPosition(const char * buffer, const char * position) const {
  assert(buffer != nullptr && position != nullptr);
  assert(position >= buffer);
  KDSize glyphSize = m_font->glyphSize();
  KDCoordinate cursorWidth = m_cursorView.minimalSizeForOptimalDisplay().width();
  KDCoordinate horizontalOffset = m_horizontalAlignment == 0.0f ? 0.0f :
    m_horizontalAlignment * (m_frame.width() - m_font->stringSize(buffer).width() - cursorWidth);
  return KDRect(
      horizontalOffset + m_font->stringSizeUntil(buffer, position).width(),
      m_verticalAlignment * (m_frame.height() - glyphSize.height()),
      glyphSize);
}

/* TextField */

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize,
    InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate, const KDFont * font,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput(parentResponder, &m_contentView),
  EditableField(inputEventHandlerDelegate),
  m_contentView(textBuffer, textBufferSize, draftTextBufferSize, font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_delegate(delegate)
{
}

void TextField::setBackgroundColor(KDColor backgroundColor) {
  ScrollView::setBackgroundColor(backgroundColor);
  m_contentView.setBackgroundColor(backgroundColor);
}

void TextField::setTextColor(KDColor textColor) {
  m_contentView.setTextColor(textColor);
}

bool TextField::isEditing() const {
  return m_contentView.isEditing();
}

size_t TextField::draftTextLength() const {
  assert(isEditing());
  return m_contentView.editedTextLength();
}

void TextField::setText(const char * text) {
  reloadScroll();
  m_contentView.setText(text);
  /* Set the cursor location here and not in ContentView::setText so that
   * TextInput::willSetCursorLocation is called. */
  setCursorLocation(m_contentView.editedText()+strlen(text));
}

void TextField::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_contentView.setAlignment(horizontalAlignment, verticalAlignment);
}

bool TextField::privateHandleEvent(Ion::Events::Event event) {
  // Handle Toolbox or Var event
  if (handleBoxEvent(event)) {
    if (!isEditing()) {
      reinitDraftTextBuffer();
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && shouldFinishEditing(event)) {
    /* If textFieldDidFinishEditing displays a pop-up (because of an unvalid
     * text for instance), the text field will call willResignFirstResponder.
     * This will call textFieldDidAbortEditing if the textfield is still editing,
     * which we do not want, as we are not really aborting edition, just
     * displaying a pop-up before returning to edition.
     * We thus set editing to false. */
    setEditing(false);
    /* We avoid copying the edited text into the other text buffer in case
     * textFieldDidFinishEditing fails - we then want to be able to abort
     * edition and find the old text back. Anyway, if textFieldDidFinishEditing
     * does not fail, it will save the editedText in a model and reload the
     * content of the textfield buffer using the very same model - that has
     * been updated. */
    if (m_delegate->textFieldDidFinishEditing(this, m_contentView.editedText(), event)) {
      // Clean draft text for next use
      reinitDraftTextBuffer();
      /* We allow overscroll to avoid calling layoutSubviews twice because the
       * content might have changed. */
      reloadScroll(true);
      return true;
    }
    setEditing(true);
    return true;
  }
  /* If a move event was not caught before, we handle it here to avoid bubbling
   * the event up. */
  if (isEditing()
      && (event == Ion::Events::Up
        || event == Ion::Events::Down
        || event == Ion::Events::Left
        || event == Ion::Events::Right))
  {
    return true;
  }
  if (event == Ion::Events::Backspace && isEditing()) {
    return removePreviousGlyph();
  }
  if (event == Ion::Events::Back && isEditing()) {
    reinitDraftTextBuffer();
    setEditing(false);
    m_delegate->textFieldDidAbortEditing(this);
    reloadScroll(true);
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    if (!removeEndOfLine()) {
      removeWholeText();
    }
    return true;
  }
  if (event == Ion::Events::Copy && !isEditing()) {
    Clipboard::sharedClipboard()->store(text());
    return true;
  }
  if (event == Ion::Events::Cut && !isEditing()) {
    Clipboard::sharedClipboard()->store(text());
    reinitDraftTextBuffer();
    setEditing(true);
    return true;
  }
  return false;
}

CodePoint TextField::XNTCodePoint(CodePoint defaultXNTCodePoint) {
  static constexpr struct { const char *name; char xnt; } sFunctions[] = {
    { "diff", 'x' }, { "int", 'x' },
    { "product", 'n' }, { "sum", 'n' }
  };
  /* Let's assume everything before the cursor is nested correctly, which is
   * reasonable if the expression is being entered left-to-right. */
  const char * text = this->text();
  assert(text == m_contentView.editedText());
  const char * location = cursorLocation();
  UTF8Decoder decoder(text, location);
  unsigned level = 0;
  while (location > text) {
    CodePoint c = decoder.previousCodePoint();
    location = decoder.stringPosition();
    switch (c) {
      case '(':
        // Check if we are skipping to the next matching '('.
        if (level > 0) {
          level--;
          break;
        }
        // Skip over whitespace.
        while (location > text && decoder.previousCodePoint() == ' ') {
          location = decoder.stringPosition();
        }
        location = decoder.stringPosition();
        // We found the next innermost function we are currently in.
        for (size_t i = 0; i < sizeof(sFunctions)/sizeof(sFunctions[0]); i++) {
          const char * name = sFunctions[i].name;
          size_t length = strlen(name);
          if ((location >= text + length) && memcmp(&text[(location - text) - length], name, length) == 0) {
            return sFunctions[i].xnt;
          }
        }
        break;
      case ',':
        // Commas encountered while skipping to the next matching '(' should be ignored.
        if (level) {
          break;
        }
        // FALLTHROUGH
      case ')':
        // Skip to the next matching '('.
        level++;
        break;
    }
  }

  // Fallback to the default
  return defaultXNTCodePoint;
}

bool TextField::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  size_t previousTextLength = strlen(text());
  bool didHandleEvent = false;
  if (privateHandleMoveEvent(event)) {
    didHandleEvent = true;
  } else if (m_delegate->textFieldDidReceiveEvent(this, event)) {
    return true;
  } else if (event.hasText()) {
    return handleEventWithText(event.text());
  } else if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::sharedClipboard()->storedText());
  } else if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    const char * previousText = m_contentView.text();
    setEditing(true);
    setText(previousText);
    didHandleEvent = true;
  }
  if (!didHandleEvent) {
    didHandleEvent = privateHandleEvent(event);
  }
  return m_delegate->textFieldDidHandleEvent(this, didHandleEvent, strlen(text()) != previousTextLength);
}

void TextField::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  return TextInput::scrollToCursor();
}

bool TextField::privateHandleMoveEvent(Ion::Events::Event event) {
  if (!isEditing()) {
    return false;
  }
  const char * draftBuffer = m_contentView.editedText();
  if (event == Ion::Events::Left && cursorLocation() > draftBuffer) {
    return TextInput::moveCursorLeft();
  }
  if (event == Ion::Events::Right && cursorLocation() < draftBuffer + draftTextLength()) {
    return TextInput::moveCursorRight();
  }
  if (event == Ion::Events::ShiftLeft) {
    return setCursorLocation(draftBuffer);
  }
  if (event == Ion::Events::ShiftRight) {
    return setCursorLocation(draftBuffer + draftTextLength());
  }
  return false;
}

bool TextField::handleEventWithText(const char * eventText, bool indentation, bool forceCursorRightOfText) {
  size_t previousTextLength = strlen(text());

  if (!isEditing()) {
    reinitDraftTextBuffer();
    setEditing(true);
  }

  assert(isEditing());

  if (eventText[0] == 0) {
    /* For instance, the event might be EXE on a non-editing text field to start
     * edition. */
    setCursorLocation(m_contentView.editedText());
    return m_delegate->textFieldDidHandleEvent(this, true, previousTextLength != 0);
  }

  // Remove the Empty code points
  constexpr int bufferSize = TextField::maxBufferSize();
  char buffer[bufferSize];
  UTF8Helper::CopyAndRemoveCodePoint(buffer, bufferSize, eventText, UCodePointEmpty);

  // Replace System parentheses (used to keep layout tree structure) by normal parentheses
  Poincare::SerializationHelper::ReplaceSystemParenthesesByUserParentheses(buffer);

  const char * nextCursorLocation = m_contentView.editedText() + draftTextLength();
  if (insertTextAtLocation(buffer, cursorLocation())) {
    /* The cursor position depends on the text as we sometimes want to position
     * the cursor at the end of the text and sometimes after the first
     * parenthesis. */
    nextCursorLocation = cursorLocation();
    if (forceCursorRightOfText) {
      nextCursorLocation+= strlen(buffer);
    } else {
      nextCursorLocation+= TextInputHelpers::CursorPositionInCommand(eventText) - eventText;
    }
  }
  setCursorLocation(nextCursorLocation);
  return m_delegate->textFieldDidHandleEvent(this, true, strlen(text()) != previousTextLength);
}

void TextField::removeWholeText() {
  reinitDraftTextBuffer();
  setEditing(true);
  reloadScroll();
}
