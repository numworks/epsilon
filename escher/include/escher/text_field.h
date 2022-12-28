#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/editable_field.h>
#include <escher/text_input.h>
#include <escher/text_field_delegate.h>
#include <string.h>

namespace Escher {
// See TODO in EditableField

/* TODO: TextField currently uses using 2 buffers:
 * - one to keep the displayed text
 * - another one to edit the text while keeping the previous text in the first
 *   buffer to be able to abort edition.
 * We could actually use only one buffer if for all textfields we implement the
 * delegate method 'textFieldDidAbortEdition' in the way that it reloads the
 * previous text from the model instead of from the textfield buffer. */

class AbstractTextField : public TextInput, public EditableField {
public:
  constexpr static int MaxBufferSize() { return ContentView::k_maxBufferSize; }

  AbstractTextField(Responder * parentResponder, View * contentView, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void willResignFirstResponder() override;
  void modalViewWillSpoilFirstResponder() override { contentView()->setStalled(true); }
  void modalViewDidRestoreFirstResponder() override { contentView()->setStalled(false); }

  // ScrollView
  void setBackgroundColor(KDColor backgroundColor) override;

  // TextInput
  void scrollToCursor() override;

  // InputEventHandler
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;

  // EditableField
  bool addXNTCodePoint(CodePoint defaultXNTCodePoint) override;
  void setEditing(bool isEditing) override { assert(!isEditing || m_delegate->textFieldIsEditable(this)); contentView()->setEditing(isEditing); }
  bool isEditing() const override;
  bool shouldFinishEditing(Ion::Events::Event event) override;

  void setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate);
  void setInputEventHandlerDelegate(InputEventHandlerDelegate * inputEventHandlerDelegate);
  void setText(const char * text);
  char * draftTextBuffer() const { return const_cast<char *>(nonEditableContentView()->editedText()); }
  size_t draftTextBufferSize() const { return nonEditableContentView()->draftTextBufferSize(); }
  size_t draftTextLength() const;
  void setDraftTextBufferSize(size_t size) { contentView()->setDraftTextBufferSize(size); }
  void reinitDraftTextBuffer() { contentView()->reinitDraftTextBuffer(); }
  KDFont::Size font() const { return nonEditableContentView()->font(); }
  void setTextColor(KDColor textColor);
  size_t insertXNTChars(CodePoint defaultXNTCodePoint, char * buffer, size_t bufferLength);
  bool cursorAtEndOfText() const { return isEditing() && cursorLocation() == text() + draftTextLength(); }
  void setEditionBuffer(char * buffer, size_t bufferSize) { contentView()->setEditionBuffer(buffer, bufferSize); }

protected:
  class ContentView : public TextInput::ContentView {
  public:
    /* In some app (ie Calculation), text fields record expression results whose
     * lengths can reach 70 (ie
     * [[1.234567e-123*e^(1.234567e-123*i), 1.234567e-123*e^(1.234567e-123*i)]]).
     * In order to be able to record those output text, k_maxBufferSize must be
     * over 70.
     * Furthermore, we want ot be able to write an adjacency matrix of size 10
     * so we need at least 2 brackets + 10 * (2 brackets + 10 digits + 9 commas)
     * = 212 characters. */
    constexpr static int k_maxBufferSize = 220;

    ContentView(char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize, KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor);

    // View
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;

    // TextInput::ContentView
    const char * text() const override;
    const char * editedText() const override;
    size_t editedTextLength() const override;
    /* If the text to be appended is too long to be added without overflowing the
     * buffer, nothing is done (not even adding few letters from the text to reach
     * the maximum buffer capacity) and false is returned. */
    bool insertTextAtLocation(const char * text, char * location, int textLength = -1) override;
    bool removePreviousGlyph() override;
    bool removeEndOfLine() override;
    size_t deleteSelection() override;

    void setBackgroundColor(KDColor backgroundColor);
    KDColor backgroundColor() const { return m_backgroundColor; }
    void setTextColor(KDColor textColor);

    bool isEditing() const { return m_isEditing; }
    void setText(const char * text);
    void setEditing(bool isEditing);
    void reinitDraftTextBuffer();
    void setDraftTextBufferSize(size_t size) { assert(size <= k_maxBufferSize); m_draftTextBufferSize = size; }
    size_t draftTextBufferSize() const { return m_draftTextBufferSize; }
    void willModifyTextBuffer();
    void didModifyTextBuffer();
    void setEditionBuffer(char * buffer, size_t bufferSize);

    void stallOrStopEditing();
    bool isStalled() const { return m_isStalled; }
    void setStalled(bool stalled) { m_isStalled = stalled; }

  protected:
    KDRect glyphFrameAtPosition(const char * buffer, const char * position) const override;

  private:
    void layoutSubviews(bool force = false) override;
    size_t editionBufferSize() const;

    char * m_textBuffer;
    size_t m_textBufferSize;
    size_t m_draftTextBufferSize;
    KDColor m_textColor;
    KDColor m_backgroundColor;
    bool m_isEditing;
    /* The textfield is 'Stalling' when the edition has been interrupted (by a
     * 'syntax error pop-up for instance) but should not be discarded. */
    bool m_isStalled;
    bool m_useDraftBuffer;
  };

  const ContentView * nonEditableContentView() const override = 0;
  ContentView * contentView() { return const_cast<ContentView *>(nonEditableContentView()); }

private:
  virtual void removeWholeText();
  void removePreviousGlyphIfRepetition(bool defaultXNTHasChanged);
  bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event);
  bool privateHandleSelectEvent(Ion::Events::Event event);
  bool handleStoreEvent() override;
  bool storeInClipboard() const;

  TextFieldDelegate * m_delegate;
};

class TextField : public AbstractTextField {
public:
  TextField(Responder * parentResponder,
      char * textBuffer,
      size_t textBufferSize,
      size_t draftTextBufferSize,
      InputEventHandlerDelegate * inputEventHandlerDelegate,
      TextFieldDelegate * delegate = nullptr,
      KDFont::Size font = KDFont::Size::Large,
      float horizontalAlignment = KDContext::k_alignLeft,
      float verticalAlignment = KDContext::k_alignCenter,
      KDColor textColor = KDColorBlack,
      KDColor backgroundColor = KDColorWhite);

protected:
  const AbstractTextField::ContentView * nonEditableContentView() const { return &m_contentView; }

  AbstractTextField::ContentView m_contentView;
};

}
#endif
