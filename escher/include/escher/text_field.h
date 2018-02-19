#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/scrollable_view.h>
#include <escher/text_field_delegate.h>
#include <escher/text_cursor_view.h>
#include <string.h>

class TextField : public ScrollableView, public ScrollViewDataSource {
public:
  TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer, size_t textBufferSize,
    TextFieldDelegate * delegate = nullptr, bool hasTwoBuffers = true, KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 0.0f,
    float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
  void setDelegate(TextFieldDelegate * delegate);
  void setDraftTextBuffer(char * draftTextBuffer);
  Toolbox * toolbox() override;
  bool isEditing() const;
  const char * text() const;
  int draftTextLength() const;
  int cursorLocation() const;
  void setCursorLocation(int location);
  void setText(const char * text);
  void setBackgroundColor(KDColor backgroundColor);
  KDColor backgroundColor() const;
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  virtual void setEditing(bool isEditing, bool reinitDraftBuffer = true);
  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity) and false is returned. */
  bool insertTextAtLocation(const char * text, int location);
  KDSize minimalSizeForOptimalDisplay() const override;
  bool handleEventWithText(const char * text);
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(Ion::Events::Event event);
  constexpr static int maxBufferSize() {
     return ContentView::k_maxBufferSize;
  }
  void scrollToCursor();
protected:
  class ContentView : public View {
  public:
    ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, KDText::FontSize size, float horizontalAlignment = 0.0f,
    float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
    void setDraftTextBuffer(char * draftTextBuffer);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reload();
    bool isEditing() const { return m_isEditing; }
    const char * text() const;
    int draftTextLength() const;
    int cursorLocation() const { return m_currentCursorLocation; }
    char * textBuffer() { return m_textBuffer; }
    char * draftTextBuffer() { return m_draftTextBuffer; }
    int bufferSize() { return k_maxBufferSize; }
    void setText(const char * text);
    void setBackgroundColor(KDColor backgroundColor);
    KDColor backgroundColor() const { return m_backgroundColor; }
    void setTextColor(KDColor textColor);
    void setAlignment(float horizontalAlignment, float verticalAlignment);
    void setEditing(bool isEditing, bool reinitDraftBuffer);
    void reinitDraftTextBuffer();
    void setCursorLocation(int location);
    bool insertTextAtLocation(const char * text, int location);
    KDSize minimalSizeForOptimalDisplay() const override;
    KDCoordinate textHeight() const;
    KDCoordinate textWidth() const;
    KDCoordinate charWidth() const;
    void deleteCharPrecedingCursor();
    bool deleteEndOfLine();
    KDRect cursorRect();
    View * subviewAtIndex(int index) override { return &m_cursorView; }
    /* In some app (ie Calculation), text fields record expression results whose
     * lengths can reach 70 (ie
     * [[1.234567e-123*e^(1.234567e-123*i), 1.234567e-123*e^(1.234567e-123*i)]]).
     * In order to be able to record those output text, k_maxBufferSize must be
     * over 70. */
    constexpr static int k_maxBufferSize = 152;
  private:
    int numberOfSubviews() const override { return 1; }
    void layoutSubviews() override;
    KDPoint textOrigin() const;
    TextCursorView m_cursorView;
    bool m_isEditing;
    char * m_textBuffer;
    char * m_draftTextBuffer;
    size_t m_currentDraftTextLength;
    size_t m_currentCursorLocation;
    size_t m_textBufferSize;
    float m_horizontalAlignment;
    float m_verticalAlignment;
    KDColor m_textColor;
    KDColor m_backgroundColor;
    KDText::FontSize m_fontSize;
  };
  ContentView m_contentView;
private:
  bool privateHandleEvent(Ion::Events::Event event);
  void deleteCharPrecedingCursor();
  bool deleteEndOfLine();
  bool m_hasTwoBuffers;
  TextFieldDelegate * m_delegate;
};

#endif
