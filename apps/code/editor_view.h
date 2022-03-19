#ifndef CODE_EDITOR_VIEW_H
#define CODE_EDITOR_VIEW_H

#include <escher.h>
#include "python_text_area.h"

namespace Code {

class EditorView : public Responder, public View, public ScrollViewDelegate {
public:
  EditorView(Responder * parentResponder, App * pythonDelegate);
  PythonTextArea::AutocompletionType autocompletionType(const char ** autocompletionBeginning, const char ** autocompletionEnd) const { return m_textArea.autocompletionType(nullptr, autocompletionBeginning, autocompletionEnd); }
  bool isAutocompleting() const;
  void resetSelection();
  void setTextAreaDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate, TextAreaDelegate * delegate) {
    m_textArea.setDelegates(inputEventHandlerDelegate, delegate);
  }
  const char * text() const { return m_textArea.text(); }
  void setText(char * textBuffer, size_t textBufferSize) {
    m_textArea.setText(textBuffer, textBufferSize);
  }
  const char * cursorLocation() {
    return m_textArea.cursorLocation();
  }
  bool setCursorLocation(const char * location) {
    return m_textArea.setCursorLocation(location);
  }
  void loadSyntaxHighlighter() { m_textArea.loadSyntaxHighlighter(); };
  void unloadSyntaxHighlighter() { m_textArea.unloadSyntaxHighlighter(); };
  void scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) override;
  void didBecomeFirstResponder() override;
  void internalLayoutSubviews(bool force);
private:
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  class GutterView : public View {
  public:
    GutterView(const KDFont * font) : View(), m_font(font), m_offset(0), m_numberOfDigits(2) {}

    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool setOffsetAndNeedResize(KDCoordinate offset); // Return true if the gutter view need to be resized

    int computeWidth();
    int computeMaxNumberOfDigits();
    static int computeNumberOfDigitsFor(int value);

  private:
    static constexpr KDCoordinate k_margin = 2;

    const KDFont * m_font;
    KDCoordinate m_offset;
    int m_numberOfDigits;
  };

  PythonTextArea m_textArea;
  GutterView m_gutterView;
};

}

#endif
