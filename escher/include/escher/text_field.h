#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/editable_field.h>
#include <escher/text_input.h>
#include <escher/text_field_delegate.h>
#include <string.h>

class TextField : public TextInput, public EditableField {
public:
  TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer, size_t textBufferSize,
    InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate = nullptr, bool hasTwoBuffers = true, const KDFont * font = KDFont::LargeFont,
    float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setBackgroundColor(KDColor backgroundColor) override;
  void setTextColor(KDColor textColor);
  void setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate) { m_inputEventHandlerDelegate = inputEventHandlerDelegate; m_delegate = delegate; }
  void setDraftTextBuffer(char * draftTextBuffer);
  bool isEditing() const override;
  const char * draftTextBuffer() const { return const_cast<TextField *>(this)->m_contentView.draftTextBuffer(); }
  size_t draftTextLength() const; //TODO keep ?
  void setText(const char * text);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  virtual void setEditing(bool isEditing, bool reinitDraftBuffer = true) override;
  CodePoint XNTCodePoint(CodePoint defaultXNTCodePoint) override;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  constexpr static int maxBufferSize() {
     return ContentView::k_maxBufferSize;
  }
  void scrollToCursor() override;
  bool shouldFinishEditing(Ion::Events::Event event) override { return m_delegate->textFieldShouldFinishEditing(this, event); }
  const KDFont * font() const { return m_contentView.font(); }
protected:
  class ContentView : public TextInput::ContentView {
  public:
    ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, const KDFont * font, float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
    void setBackgroundColor(KDColor backgroundColor);
    KDColor backgroundColor() const { return m_backgroundColor; }
    void setTextColor(KDColor textColor);
    void setDraftTextBuffer(char * draftTextBuffer);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool isEditing() const { return m_isEditing; }
    const char * text() const override;
    const char * editedText() const override { return m_draftTextBuffer; }
    size_t editedTextLength() const override { return m_currentDraftTextLength; } //TODO keep ?
    char * textBuffer() { return m_textBuffer; }
    char * draftTextBuffer() { return m_draftTextBuffer; }
    int bufferSize() { return k_maxBufferSize; }
    void setText(const char * text);
    void setAlignment(float horizontalAlignment, float verticalAlignment);
    void setEditing(bool isEditing, bool reinitDraftBuffer);
    void reinitDraftTextBuffer();
    /* If the text to be appended is too long to be added without overflowing the
     * buffer, nothing is done (not even adding few letters from the text to reach
     * the maximum buffer capacity) and false is returned. */
    bool insertTextAtLocation(const char * text, const char * location) override; // TODO
    KDSize minimalSizeForOptimalDisplay() const override;
    bool removePreviousGlyph() override;
    bool removeEndOfLine() override;
    void willModifyTextBuffer();
    void didModifyTextBuffer();
    /* In some app (ie Calculation), text fields record expression results whose
     * lengths can reach 70 (ie
     * [[1.234567e-123*e^(1.234567e-123*i), 1.234567e-123*e^(1.234567e-123*i)]]).
     * In order to be able to record those output text, k_maxBufferSize must be
     * over 70.
     * Furthermore, we want ot be able to write an adjacency matrix of size 10
     * so we need at least 2 brackets + 10 * (2 brackets + 10 digits + 9 commas)
     * = 212 characters. */
    constexpr static int k_maxBufferSize = 220;
  private:
    void layoutSubviews() override;
    KDRect glyphFrameAtPosition(const char * buffer, const char * position) const override;
    bool m_isEditing;
    char * m_textBuffer;
    char * m_draftTextBuffer;
    size_t m_currentDraftTextLength; //TODO keep ?
    size_t m_textBufferSize;
    float m_horizontalAlignment;
    float m_verticalAlignment;
    KDColor m_textColor;
    KDColor m_backgroundColor;
  };
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
private:
  bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event);
  virtual void removeWholeText();
  bool m_hasTwoBuffers;
  TextFieldDelegate * m_delegate;
};

#endif
