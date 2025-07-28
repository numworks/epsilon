#ifndef ESCHER_ABSTRACT_TEXT_FIELD_H
#define ESCHER_ABSTRACT_TEXT_FIELD_H

#include <escher/text_field_delegate.h>
#include <escher/text_input.h>
#include <string.h>

namespace Escher {

/* TODO: TextField currently uses using 2 buffers:
 * - one to keep the displayed text
 * - another one to edit the text while keeping the previous text in the first
 *   buffer to be able to abort edition.
 * We could actually use only one buffer if for all textfields we implement the
 * delegate method 'textFieldDidAbortEdition' in the way that it reloads the
 * previous text from the model instead of from the textfield buffer. */

class AbstractTextField : public TextInput {
 public:
  constexpr static int MaxBufferSize() { return ContentView::k_maxBufferSize; }

  AbstractTextField(Responder* parentResponder, View* contentView,
                    TextFieldDelegate* delegate);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ScrollView
  void setBackgroundColor(KDColor backgroundColor) override;

  // TextInput
  void scrollToCursor() override;

  // EditableField
  bool handleEventWithText(const char* text, bool indentation = false,
                           bool forceCursorRightOfText = false) override;
  bool handleEventWithLayout(Poincare::Layout layout,
                             bool forceCursorRightOfText = false) override;
  void setEditing(bool isEditing) {
    assert(!isEditing || isEditable());
    contentView()->setEditing(isEditing);
  }

  bool isEditing() const { return nonEditableContentView()->isEditing(); }
  bool isEditable() {
    return m_delegate ? m_delegate->textFieldIsEditable(this) : true;
  }
  void setDelegate(TextFieldDelegate* delegate) { m_delegate = delegate; }
  void setText(const char* text);
  char* draftText() const {
    return const_cast<char*>(nonEditableContentView()->draftText());
  }
  size_t draftTextLength() const {
    assert(isEditing());
    return nonEditableContentView()->draftTextLength();
  }
  char* draftTextEnd() const {
    return const_cast<char*>(nonEditableContentView()->draftTextEnd());
  }
  void reinitDraftTextBuffer() { contentView()->reinitDraftTextBuffer(); }
  KDFont::Size font() const { return nonEditableContentView()->font(); }
  void setTextColor(KDColor textColor) {
    contentView()->setTextColor(textColor);
  }
  bool cursorAtEndOfText() const {
    return isEditing() && cursorLocation() == draftTextEnd();
  }
  size_t dumpContent(char* buffer, size_t bufferSize, int* cursorOffset);

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

    ContentView(char* textBuffer, size_t textBufferSize,
                KDGlyph::Format format);

    // View
    void drawRect(KDContext* ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;

    // TextInput::ContentView
    const char* text() const override;
    const char* draftText() const override;
    /* If the text to be appended is too long to be added without overflowing
     * the buffer, nothing is done (not even adding few letters from the text to
     * reach the maximum buffer capacity) and false is returned. */
    bool insertTextAtLocation(const char* text, char* location,
                              int textLength = -1) override;
    bool removePreviousGlyph() override;
    bool removeEndOfLine() override;
    size_t deleteSelection() override;

    // Format
    void setBackgroundColor(KDColor backgroundColor);
    KDColor backgroundColor() const { return m_format.style.backgroundColor; }
    void setTextColor(KDColor textColor);

    bool isEditing() const { return m_isEditing; }
    void setText(const char* text);
    void setEditing(bool isEditing);
    void reinitDraftTextBuffer();
    size_t draftTextBufferSize() const { return MaxBufferSize(); }
    void willModifyTextBuffer();
    void didModifyTextBuffer();

    void stallOrStopEditing();
    bool isStalled() const { return m_isStalled; }
    void setStalled(bool stalled) { m_isStalled = stalled && m_isEditing; }

    KDRect cursorRect() const override;

   protected:
    KDRect glyphFrameAtPosition(const char* buffer,
                                const char* position) const override;

   private:
    char* m_textBuffer;
    size_t m_textBufferSize;
    bool m_isEditing;
    /* The textfield is 'Stalling' when the edition has been interrupted (by a
     * 'syntax error pop-up for instance) but should not be discarded. */
    bool m_isStalled;
  };

  void handleResponderChainEvent(Responder::ResponderChainEvent event) override;
  const ContentView* nonEditableContentView() const override = 0;
  ContentView* contentView() {
    return const_cast<ContentView*>(nonEditableContentView());
  }

 private:
  void privateModalViewAltersFirstResponder(
      Responder::FirstResponderAlteration alteration) override {
    contentView()->setStalled(alteration ==
                              Responder::FirstResponderAlteration::WillSpoil);
  }
  bool prepareToEdit() override;
  bool findXNT(char* buffer, size_t bufferSize, int xntIndex,
               size_t* cycleSize) override;
  void removePreviousXNT() override;

  virtual void removeWholeText();
  bool handleMoveEvent(Ion::Events::Event event);
  bool handleSelectEvent(Ion::Events::Event event);
  bool handleStoreEvent() override;
  bool storeInClipboard() const;
  bool privateHandleEvent(Ion::Events::Event event, bool* textDidChange);
  bool insertText(const char* text, bool indentation = false,
                  bool forceCursorRightOfText = false);
  size_t getTextFromEvent(Ion::Events::Event event, char* buffer,
                          size_t bufferSize) override;

  TextFieldDelegate* m_delegate;
};

}  // namespace Escher

#endif
