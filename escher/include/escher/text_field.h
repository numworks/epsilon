#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/editable_field.h>
#include <escher/text_input.h>
#include <escher/text_field_delegate.h>
#include <string.h>

// See TODO in EditableField

/* TODO: TextField currently uses using 2 buffers:
 * - one to keep the displayed text
 * - another one to edit the text while keeping the previous text in the first
 *   buffer to be able to abort edition.
 * We could actually use only one buffer if for all textfields we implement the
 * delegate method 'textFieldDidAbortEdition' in the way that it reloads the
 * previous text from the model instead of from the textfield buffer. */

class TextField : public TextInput, public EditableField {
public:
  TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize,
    InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate = nullptr,
    const KDFont * font = KDFont::LargeFont, float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f,
    KDColor textColor = Palette::PrimaryText, KDColor backgroundColor = Palette::BackgroundHard);
  void setBackgroundColor(KDColor backgroundColor) override;
  void setTextColor(KDColor textColor);
  void setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate) { m_inputEventHandlerDelegate = inputEventHandlerDelegate; m_delegate = delegate; }
  void reinitDraftTextBuffer() { m_contentView.reinitDraftTextBuffer(); }
  bool isEditing() const override;
  char * draftTextBuffer() const { return const_cast<char *>(m_contentView.editedText()); }
  void setDraftTextBufferSize(size_t size) { m_contentView.setDraftTextBufferSize(size); }
  size_t draftTextBufferSize() const { return m_contentView.draftTextBufferSize(); }
  size_t draftTextLength() const;
  void setText(const char * text);
  void setEditing(bool isEditing) override { m_contentView.setEditing(isEditing); }
  CodePoint XNTCodePoint(CodePoint defaultXNTCodePoint) override;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false, bool shouldRemoveLastCharacter = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  constexpr static int maxBufferSize() {
     return ContentView::k_maxBufferSize;
  }
  void scrollToCursor() override;
  // TODO: factorize with TextField (see TODO of EditableField)
  bool shouldFinishEditing(Ion::Events::Event event) override;
  const KDFont * font() const { return m_contentView.font(); }
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

    ContentView(char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize, const KDFont * font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor);
    void setBackgroundColor(KDColor backgroundColor);
    KDColor backgroundColor() const { return m_backgroundColor; }
    void setTextColor(KDColor textColor);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool isEditing() const { return m_isEditing; }
    const char * text() const override;
    const char * editedText() const override;
    size_t editedTextLength() const override { return m_currentDraftTextLength; }
    void setText(const char * text);
    void setEditing(bool isEditing);
    void reinitDraftTextBuffer();
    void setDraftTextBufferSize(size_t size) { assert(size <= k_maxBufferSize); m_draftTextBufferSize = size; }
    size_t draftTextBufferSize() const { return m_draftTextBufferSize; }
    bool isAbleToInsertTextAt(int textLength, const char * location, bool shouldRemoveLastCharacter) const override;
    /* If the text to be appended is too long to be added without overflowing the
     * buffer, nothing is done (not even adding few letters from the text to reach
     * the maximum buffer capacity) and false is returned. */
    void insertTextAtLocation(const char * text, char * location, int textLength = -1) override;
    KDSize minimalSizeForOptimalDisplay() const override;
    bool removePreviousGlyph() override;
    bool removeEndOfLine() override;
    void willModifyTextBuffer();
    void didModifyTextBuffer();
    size_t deleteSelection() override;
  private:
    void layoutSubviews(bool force = false) override;
    KDRect glyphFrameAtPosition(const char * buffer, const char * position) const override;
    bool m_isEditing;
    char * m_textBuffer;
    size_t m_textBufferSize;
    size_t m_draftTextBufferSize;
    size_t m_currentDraftTextLength;
    KDColor m_textColor;
    KDColor m_backgroundColor;
  };

  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;

private:
  bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event);
  bool privateHandleSelectEvent(Ion::Events::Event event);
  virtual void removeWholeText();
  bool storeInClipboard() const;
  TextFieldDelegate * m_delegate;
};

#endif
