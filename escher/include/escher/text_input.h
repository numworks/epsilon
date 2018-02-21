#ifndef ESCHER_TEXT_INPUT_H
#define ESCHER_TEXT_INPUT_H

#include <assert.h>
#include <string.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <escher/text_input_delegate.h>

class TextInput : public ScrollableView, public ScrollViewDataSource {
public:
  TextInput(Responder * parentResponder, View * contentView);
  Toolbox * toolbox() override;
  const char * text() const { return nonEditableContentView()->text(); }
  void setBackgroundColor(KDColor backgroundColor);
  KDColor backgroundColor() const { return nonEditableContentView()->backgroundColor(); }
  void setTextColor(KDColor textColor);
  bool removeChar();
  size_t cursorLocation() const { return nonEditableContentView()->cursorLocation(); }
  bool setCursorLocation(int location);
  virtual void scrollToCursor();
  virtual bool handleEventWithText(const char * text, bool indenting = false) = 0;
protected:
  class ContentView : public View {
  public:
    ContentView(KDText::FontSize size, KDColor textColor, KDColor backgroundColor);
    void setBackgroundColor(KDColor backgroundColor);
    KDColor backgroundColor() const { return m_backgroundColor; }
    void setTextColor(KDColor textColor);
    size_t cursorLocation() const { return m_cursorIndex; }
    void setCursorLocation(int cursorLocation);
    virtual const char * text() const = 0;
    virtual bool insertTextAtLocation(const char * text, int location) = 0;
    virtual bool removeChar() = 0;
    virtual bool removeEndOfLine() = 0;
    KDRect cursorRect();
  protected:
    virtual void layoutSubviews() override;
    void reloadRectFromCursorPosition(size_t index, bool lineBreak = false);
    virtual KDRect characterFrameAtIndex(size_t index) const = 0;
    TextCursorView m_cursorView;
    KDText::FontSize m_fontSize;
    KDColor m_textColor;
    KDColor m_backgroundColor;
    size_t m_cursorIndex;
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    virtual size_t editedTextLength() const = 0;
  };
protected:
  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity) and false is returned. */
  bool insertTextAtLocation(const char * textBuffer, int location);
  virtual bool removeEndOfLine();
  ContentView * contentView() {
    return const_cast<ContentView *>(nonEditableContentView());
  }
  virtual const ContentView * nonEditableContentView() const = 0;
private:
  virtual TextInputDelegate * delegate() = 0;
};

#endif
