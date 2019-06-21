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
  bool removePreviousGlyph();
  const char * cursorLocation() const { return nonEditableContentView()->cursorLocation(); }
  bool setCursorLocation(const char * location);
  virtual void scrollToCursor();
protected:
  class ContentView : public View {
  public:
    ContentView(const KDFont * font) :
      View(),
      m_cursorView(),
      m_font(font),
      m_cursorLocation(nullptr)
    {}
    void setFont(const KDFont * font);
    const KDFont * font() const { return m_font; }
    const char * cursorLocation() const { assert(m_cursorLocation != nullptr); return m_cursorLocation; }
    void setCursorLocation(const char * cursorLocation);
    virtual const char * text() const = 0;
    virtual bool insertTextAtLocation(const char * text, const char * location) = 0;
    virtual bool removePreviousGlyph() = 0;
    virtual bool removeEndOfLine() = 0;
    KDRect cursorRect();
  protected:
    virtual void layoutSubviews() override;
    void reloadRectFromPosition(const char * position, bool lineBreak = false);
    virtual KDRect glyphFrameAtPosition(const char * buffer, const char * position) const = 0;
    TextCursorView m_cursorView;
    const KDFont * m_font;
    const char * m_cursorLocation;
    virtual KDRect dirtyRectFromPosition(const char * position, bool lineBreak) const;
  private:
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_cursorView;
    }
    virtual const char * editedText() const = 0;
    virtual size_t editedTextLength() const = 0;
  };

  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity) and false is returned. */
  bool insertTextAtLocation(const char * textBuffer, const char * location);
  bool removeEndOfLine();
  ContentView * contentView() {
    return const_cast<ContentView *>(nonEditableContentView());
  }
  virtual const ContentView * nonEditableContentView() const = 0;
  bool moveCursorLeft();
  bool moveCursorRight();
private:
  virtual void willSetCursorLocation(const char * * location) {}
  virtual bool privateRemoveEndOfLine();
};

#endif
