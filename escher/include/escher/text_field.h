#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/scrollable_view.h>
#include <escher/text_field_delegate.h>
#include <escher/text_cursor_view.h>
#include <string.h>

class TextField : public ScrollableView {
public:
  TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer, size_t textBufferSize,
    TextFieldDelegate * delegate = nullptr, KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 0.0f,
    float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
  Toolbox * toolbox() override;
  bool isEditing() const;
  const char * text() const;
  int textLength() const;
  int cursorLocation() const;
  void setText(const char * text);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  virtual void setEditing(bool isEditing, bool reinitDraftBuffer = true);
  void setCursorLocation(int location);
  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity. */
  void insertTextAtLocation(const char * text, int location);
  KDSize minimalSizeForOptimalDisplay() const override;
  void setTextFieldDelegate(TextFieldDelegate * delegate);
  bool handleEvent(Ion::Events::Event event) override;
protected:
  class ContentView : public View {
  public:
    ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, KDText::FontSize size, float horizontalAlignment = 0.0f,
    float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reload();
    bool isEditing() const;
    const char * text() const;
    int textLength() const;
    int cursorLocation() const;
    char * textBuffer();
    char * draftTextBuffer();
    int bufferSize();
    void setText(const char * text);
    void setBackgroundColor(KDColor backgroundColor);
    void setTextColor(KDColor textColor);
    void setAlignment(float horizontalAlignment, float verticalAlignment);
    void setEditing(bool isEditing, bool reinitDraftBuffer);
    void reinitDraftTextBuffer();
    void setCursorLocation(int location);
    void insertTextAtLocation(const char * text, int location);
    KDSize minimalSizeForOptimalDisplay() const override;
    KDCoordinate textHeight() const;
    KDCoordinate charWidth();
    void deleteCharPrecedingCursor();
    View * subviewAtIndex(int index) override;
    constexpr static int k_maxBufferSize = 255;
  private:
    int numberOfSubviews() const override;
    void layoutSubviews() override;
    TextCursorView m_cursorView;
    bool m_isEditing;
    char * m_textBuffer;
    char * m_draftTextBuffer;
    size_t m_currentTextLength;
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
  void deleteCharPrecedingCursor();
  bool cursorIsBeforeScrollingFrame();
  bool cursorIsAfterScrollingFrame();
  void scrollToCursor();
  void scrollToAvoidWhiteSpace();
  View * view() override;
  TextFieldDelegate * m_delegate;
};

#endif
