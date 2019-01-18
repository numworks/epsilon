#ifndef ESCHER_TEXT_INPUT_H
#define ESCHER_TEXT_INPUT_H

#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <assert.h>
#include <string.h>

class TextInput : public ScrollableView, public ScrollViewDataSource {
public:
  TextInput(Responder * parentResponder, View * contentView) : ScrollableView(parentResponder, contentView, this) {}
  void setFont(const KDFont * font) { contentView()->setFont(font); }
  const char * text() const { return nonEditableContentView()->text(); }
  bool removeChar();
  size_t cursorLocation() const { return nonEditableContentView()->cursorLocation(); }
  bool setCursorLocation(int location);
  virtual void scrollToCursor();
protected:
  class ContentView : public View {
  public:
    ContentView(const KDFont * font) :
      View(),
      m_cursorView(),
      m_font(font),
      m_cursorIndex(0)
    {}
    void setFont(const KDFont * font);
    const KDFont * font() const { return m_font; }
    size_t cursorLocation() const { return m_cursorIndex; }
    void setCursorLocation(int cursorLocation);
    virtual const char * text() const = 0;
    virtual bool insertTextAtLocation(const char * text, int location) = 0;
    virtual bool removeChar() = 0;
    virtual bool removeEndOfLine() = 0;
    KDRect cursorRect();
  protected:
    virtual void layoutSubviews() override;
    virtual KDRect dirtyRectFromCursorPosition(size_t index, bool lineBreak) const;
    void reloadRectFromCursorPosition(size_t index, bool lineBreak = false);
    virtual KDRect characterFrameAtIndex(size_t index) const = 0;
    TextCursorView m_cursorView;
    const KDFont * m_font;
    size_t m_cursorIndex;
  private:
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 1);
      return &m_cursorView;
    }
    virtual size_t editedTextLength() const = 0;
  };
protected:
  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity) and false is returned. */
  bool insertTextAtLocation(const char * textBuffer, int location);
  bool removeEndOfLine();
  ContentView * contentView() {
    return const_cast<ContentView *>(nonEditableContentView());
  }
  virtual const ContentView * nonEditableContentView() const = 0;
private:
  virtual void willSetCursorLocation(int * location) {}
  virtual bool privateRemoveEndOfLine();
};

#endif
