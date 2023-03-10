#ifndef ESCHER_TEXT_INPUT_H
#define ESCHER_TEXT_INPUT_H

#include <assert.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <omg/directions.h>
#include <string.h>

// See TODO in EditableField

namespace Escher {

class TextInput
    : public WithBlinkingTextCursor<ScrollableView<ScrollView::NoDecorator>>,
      public ScrollViewDataSource {
  friend class ExpressionField;

 public:
  TextInput(Responder *parentResponder, View *contentView)
      : WithBlinkingTextCursor<ScrollableView>(parentResponder, contentView,
                                               this) {}
  void setFont(KDFont::Size font) { contentView()->setFont(font); }
  const char *text() const { return nonEditableContentView()->text(); }
  bool removePreviousGlyph();
  const char *cursorLocation() const {
    return nonEditableContentView()->cursorLocation();
  }
  bool setCursorLocation(const char *location);
  virtual void scrollToCursor();
  // Selection
  bool selectionIsEmpty() const {
    return nonEditableContentView()->selectionIsEmpty();
  }
  void resetSelection() { contentView()->resetSelection(); }
  void deleteSelection();
  // Alignment
  void setAlignment(float horizontalAlignment, float verticalAlignment);

 protected:
  class ContentView : public View {
   public:
    ContentView(KDFont::Size font,
                float horizontalAlignment = KDGlyph::k_alignLeft,
                float verticalAlignment = KDGlyph::k_alignCenter)
        : View(),
          m_cursorView(this),
          m_cursorLocation(nullptr),
          m_selectionStart(nullptr),
          m_horizontalAlignment(horizontalAlignment),
          m_verticalAlignment(verticalAlignment),
          m_font(font) {}

    // Font
    void setFont(KDFont::Size font);
    KDFont::Size font() const { return m_font; }

    // Cursor location
    const char *cursorLocation() const {
      assert(m_cursorLocation != nullptr);
      return m_cursorLocation;
    }
    void setCursorLocation(const char *cursorLocation);
    KDRect cursorRect();
    TextCursorView *textCursorView() { return &m_cursorView; }

    // Virtual text get/add/remove
    virtual const char *text() const = 0;
    virtual bool insertTextAtLocation(const char *text, char *location,
                                      int textLength = -1) = 0;
    virtual bool removePreviousGlyph() = 0;
    virtual bool removeEndOfLine() = 0;

    // Selection
    const char *selectionLeft() const {
      return std::min(m_selectionStart, m_cursorLocation);
    }
    const char *selectionRight() const {
      return std::max(m_selectionStart, m_cursorLocation);
    }
    void updateSelection(const char *previousCursorLocation);
    bool resetSelection();  // returns true if the selection was indeed reset
    bool selectionIsEmpty() const;
    virtual size_t deleteSelection() = 0;

    // Alignment
    void setAlignment(float horizontalAlignment, float verticalAlignment);
    float horizontalAlignment() const { return m_horizontalAlignment; }

    // Reload
    void reloadRectFromPosition(const char *position,
                                bool includeFollowingLines = false);

   protected:
    void layoutSubviews(bool force = false) override;
    void reloadRectFromAndToPositions(const char *start, const char *end);
    virtual KDRect glyphFrameAtPosition(const char *buffer,
                                        const char *position) const = 0;
    virtual KDRect dirtyRectFromPosition(const char *position,
                                         bool includeFollowingLines) const;
    TextCursorView m_cursorView;
    const char *m_cursorLocation;
    const char *m_selectionStart;
    float m_horizontalAlignment;
    float m_verticalAlignment;
    KDFont::Size m_font;

   private:
    int numberOfSubviews() const override { return 1; }
    View *subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_cursorView;
    }
    virtual const char *editedText() const = 0;
    virtual size_t editedTextLength() const = 0;
  };

  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity) and false is returned. */
  bool insertTextAtLocation(const char *textBuffer, char *location);
  bool removeEndOfLine();
  ContentView *contentView() {
    return const_cast<ContentView *>(nonEditableContentView());
  }
  virtual const ContentView *nonEditableContentView() const = 0;
  bool moveCursorLeft(int step = 1);
  bool moveCursorRight(int step = 1);
  // all indicates if all the text on the left/right should be selected
  bool selectLeftRight(OMG::HorizontalDirection direction, bool all,
                       int step = 1);

 private:
  virtual void willSetCursorLocation(const char **location) {}
  virtual bool privateRemoveEndOfLine();
  TextCursorView *textCursorView() override {
    return contentView()->textCursorView();
  }
};

}  // namespace Escher
#endif
