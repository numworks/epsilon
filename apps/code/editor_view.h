#ifndef CODE_EDITOR_VIEW_H
#define CODE_EDITOR_VIEW_H

#include <escher/view.h>
#include <escher/solid_text_area.h>

class EditorView : public Responder, public View, public ScrollViewDelegate {
public:
  EditorView(Responder * parentResponder);
  void setTextAreaDelegate(TextAreaDelegate * delegate) {
    m_textArea.setDelegate(delegate);
  }
  const char * text() const {
    return m_textArea.text();
  }
  void setText(char * textBuffer, size_t textBufferSize) {
    m_textArea.setText(textBuffer, textBufferSize);
  }
  bool setCursorLocation(int location) {
    return m_textArea.setCursorLocation(location);
  }
  void scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) override;
  void didBecomeFirstResponder() override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  class GutterView : public View {
  public:
    GutterView(KDText::FontSize fontSize);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setOffset(KDCoordinate offset);
    KDSize minimalSizeForOptimalDisplay() const override;
  private:
    static constexpr KDCoordinate k_margin = 2;
    KDText::FontSize m_fontSize;
    KDCoordinate m_offset;
  };

  SolidTextArea m_textArea;
  GutterView m_gutterView;
};

#endif
