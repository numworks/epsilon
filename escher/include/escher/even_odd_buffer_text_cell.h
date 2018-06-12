#ifndef ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H
#define ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/buffer_text_view.h>

class EvenOddBufferTextCell : public EvenOddCell {
public:
  EvenOddBufferTextCell(KDText::FontSize size = KDText::FontSize::Small, float horizontalAlignment = 1.0f, float verticalAlignment = 0.5f);
  const char * text() const override;
  void setFontSize(KDText::FontSize fontSize) {
    m_bufferTextView.setFontSize(fontSize);
  }
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_bufferTextView.setAlignment(horizontalAlignment, verticalAlignment);
  }
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setText(const char * textContent);
  void setTextColor(KDColor textColor);
protected:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;  BufferTextView m_bufferTextView;
};

#endif
