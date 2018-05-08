#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/text_input.h>
#include <escher/text_field_delegate.h>
#include <string.h>

class TextField : public TextInput {
public:
  TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer, size_t textBufferSize,
    TextFieldDelegate * delegate = nullptr, bool hasTwoBuffers = true, KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 0.0f,
    float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
  void setDelegate(TextFieldDelegate * delegate) { m_delegate = delegate; }
  void setDraftTextBuffer(char * draftTextBuffer);
  bool isEditing() const;
  size_t draftTextLength() const;
  void setText(const char * text);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  virtual void setEditing(bool isEditing, bool reinitDraftBuffer = true);
  KDSize minimalSizeForOptimalDisplay() const override;
  bool handleEventWithText(const char * text, bool indenting = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  constexpr static int maxBufferSize() {
     return ContentView::k_maxBufferSize;
  }
  void scrollToCursor() override;
  bool textFieldShouldFinishEditing(Ion::Events::Event event) { return m_delegate->textFieldShouldFinishEditing(this, event); }
protected:
  class ContentView : public TextInput::ContentView {
  public:
    ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, KDText::FontSize size, float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
    void setDraftTextBuffer(char * draftTextBuffer);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool isEditing() const { return m_isEditing; }
    const char * text() const override;
    size_t editedTextLength() const override;
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
    bool insertTextAtLocation(const char * text, int location) override;
    KDSize minimalSizeForOptimalDisplay() const override;
    bool removeChar() override;
    bool removeEndOfLine() override;
    /* In some app (ie Calculation), text fields record expression results whose
     * lengths can reach 70 (ie
     * [[1.234567e-123*e^(1.234567e-123*i), 1.234567e-123*e^(1.234567e-123*i)]]).
     * In order to be able to record those output text, k_maxBufferSize must be
     * over 70. */
    constexpr static int k_maxBufferSize = 152;
  private:
    void layoutSubviews() override;
    KDRect characterFrameAtIndex(size_t index) const override;
    bool m_isEditing;
    char * m_textBuffer;
    char * m_draftTextBuffer;
    size_t m_currentDraftTextLength;
    size_t m_textBufferSize;
    float m_horizontalAlignment;
    float m_verticalAlignment;
  };
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
private:
  bool privateHandleEvent(Ion::Events::Event event);
  TextInputDelegate * delegate() override {
    return m_delegate;
  }
  bool m_hasTwoBuffers;
  TextFieldDelegate * m_delegate;
};

#endif
