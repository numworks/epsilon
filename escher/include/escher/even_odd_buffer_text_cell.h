#ifndef ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H
#define ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/buffer_text_view.h>

class EvenOddBufferTextCell final : public EvenOddCell {
public:
  EvenOddBufferTextCell(KDText::FontSize size = KDText::FontSize::Small, float horizontalAlignment = 1.0f, float verticalAlignment = 0.5f);
  const char * text() { return m_bufferTextView.text(); }
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setText(const char * textContent);
  void setTextColor(KDColor textColor) { m_bufferTextView.setTextColor(textColor); }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  BufferTextView m_bufferTextView;
};

#endif
