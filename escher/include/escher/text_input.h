#ifndef ESCHER_TEXT_INPUT_H
#define ESCHER_TEXT_INPUT_H

#include <assert.h>
#include <escher/editable_field.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <omg/directions.h>
#include <string.h>

namespace Escher {

class TextInput : public EditableField {
  friend class LayoutField;

 public:
  using EditableField::EditableField;

  const char* text() const { return nonEditableContentView()->text(); }
  bool removePreviousGlyph();
  const char* cursorLocation() const {
    return nonEditableContentView()->cursorLocation();
  }
  bool setCursorLocation(const char* location);
  virtual void scrollToCursor();
  // Selection
  bool selectionIsEmpty() const {
    return nonEditableContentView()->selectionIsEmpty();
  }
  void resetSelection() { contentView()->resetSelection(); }
  void deleteSelection();

 protected:
  class ContentView : public TextCursorView::CursorFieldView {
   public:
    ContentView(KDGlyph::Format format)
        : TextCursorView::CursorFieldView(),
          m_cursorLocation(nullptr),
          m_selectionStart(nullptr),
          m_format(format) {}

    // Format
    KDFont::Size font() const { return m_format.style.font; }
    float horizontalAlignment() const { return m_format.horizontalAlignment; }

    // Cursor location
    const char* cursorLocation() const {
      assert(m_cursorLocation != nullptr);
      return m_cursorLocation;
    }
    void setCursorLocation(const char* cursorLocation);
    KDRect cursorRect() const override;

    // Virtual text get/add/remove
    virtual const char* text() const = 0;
    virtual bool insertTextAtLocation(const char* text, char* location,
                                      int textLength = -1) = 0;
    virtual bool removePreviousGlyph() = 0;
    virtual bool removeEndOfLine() = 0;

    // Selection
    const char* selectionLeft() const {
      return std::min(m_selectionStart, m_cursorLocation);
    }
    const char* selectionRight() const {
      return std::max(m_selectionStart, m_cursorLocation);
    }
    void updateSelection(const char* previousCursorLocation);
    bool resetSelection();  // returns true if the selection was indeed reset
    bool selectionIsEmpty() const;
    virtual size_t deleteSelection() = 0;

    // Reload
    void reloadRectFromPosition(const char* position,
                                bool includeFollowingLines = false);

    virtual const char* draftText() const = 0;
    size_t draftTextLength() const { return strlen(draftText()); }
    const char* draftTextEnd() const { return draftText() + draftTextLength(); }

   protected:
    void reloadRectFromAndToPositions(const char* start, const char* end);
    virtual KDRect glyphFrameAtPosition(const char* buffer,
                                        const char* position) const = 0;
    virtual KDRect dirtyRectFromPosition(const char* position,
                                         bool includeFollowingLines) const;

    const char* m_cursorLocation;
    const char* m_selectionStart;
    KDGlyph::Format m_format;
  };

  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity) and false is returned. */
  bool insertTextAtLocation(const char* textBuffer, char* location);
  bool removeEndOfLine();
  ContentView* contentView() {
    return const_cast<ContentView*>(nonEditableContentView());
  }
  virtual const ContentView* nonEditableContentView() const = 0;
  bool moveCursorLeft(int step = 1);
  bool moveCursorRight(int step = 1);
  // all indicates if all the text on the left/right should be selected
  bool selectLeftRight(OMG::HorizontalDirection direction, bool all,
                       int step = 1);

 private:
  virtual void willSetCursorLocation(const char** location) {}
  virtual bool privateRemoveEndOfLine();
  TextCursorView::CursorFieldView* cursorCursorFieldView() override {
    return contentView();
  }
};

}  // namespace Escher
#endif
