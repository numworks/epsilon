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
  void resetSelection() { contentView()->resetSelection(); }
protected:
  class ContentView : public View {
  public:
    ContentView(const KDFont * font) :
      View(),
      m_selectionStart(nullptr),
      m_selectionEnd(nullptr),
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
    const char * selectionStart() const { return m_selectionStart; }
    const char * selectionEnd() const { return m_selectionEnd; }
    void addSelection(const char * left, const char * right);
    bool resetSelection(); // returns true if the selection was indeed reset
    bool selectionIsEmpty() const;
    virtual void deleteSelectedText() {} //TODO LEA
    const char * m_selectionStart;
    const char * m_selectionEnd;
  protected:
    virtual void layoutSubviews(bool force = false) override;
    void reloadRectFromPosition(const char * position, bool includeFollowingLines = false);
    void reloadRectFromAndToPositions(const char * start, const char * end);
    virtual KDRect glyphFrameAtPosition(const char * buffer, const char * position) const = 0;
    virtual KDRect dirtyRectFromPosition(const char * position, bool includeFollowingLines) const;
    TextCursorView m_cursorView;
    const KDFont * m_font;
    const char * m_cursorLocation;
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
  bool selectLeftRight(bool left);
private:
  virtual void willSetCursorLocation(const char * * location) {}
  virtual bool privateRemoveEndOfLine();
};

#endif
