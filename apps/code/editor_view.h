#ifndef CODE_EDITOR_VIEW_H
#define CODE_EDITOR_VIEW_H

#include <escher/container.h>

#include "python_text_area.h"

namespace Code {

class EditorView : public Escher::Responder,
                   public Escher::View,
                   public Escher::ScrollViewDataSourceDelegate {
 public:
  EditorView(Escher::Responder* parentResponder, App* pythonDelegate);
  bool isAutocompleting() const;
  void resetSelection();
  void removeAutocompletionText();
  const char* text() const { return m_textArea.text(); }
  void setText(char* textBuffer, size_t textBufferSize) {
    m_textArea.setText(textBuffer, textBufferSize);
  }
  const char* cursorLocation() { return m_textArea.cursorLocation(); }
  bool setCursorLocation(const char* location) {
    return m_textArea.setCursorLocation(location);
  }
  void loadSyntaxHighlighter() { m_textArea.loadSyntaxHighlighter(); };
  void unloadSyntaxHighlighter() { m_textArea.unloadSyntaxHighlighter(); };
  void scrollViewDidChangeOffset(
      Escher::ScrollViewDataSource* scrollViewDataSource) override;
  void didBecomeFirstResponder() override;

 private:
  int numberOfSubviews() const override { return 2; }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  class GutterView : public View {
   public:
    GutterView(KDFont::Size font) : View(), m_font(font), m_offset(0) {}
    void drawRect(KDContext* ctx, KDRect rect) const override;
    void setOffset(KDCoordinate offset);
    KDSize minimalSizeForOptimalDisplay() const override;

   private:
    constexpr static KDCoordinate k_margin = 2;
    constexpr static int k_lineNumberCharLength = 2;
    KDFont::Size m_font;
    KDCoordinate m_offset;
  };

  PythonTextArea m_textArea;
  GutterView m_gutterView;
};

}  // namespace Code

#endif
