#ifndef ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H
#define ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/buffer_text_view.h>
#include <escher/metric.h>

class EvenOddBufferTextCell : public EvenOddCell {
public:
  EvenOddBufferTextCell(const KDFont * font = KDFont::SmallFont, float horizontalAlignment = 1.0f, float verticalAlignment = 0.5f);
  const char * text() const override;
  void setFont(const KDFont * font) {
    m_bufferTextView.setFont(font);
  }
  const KDFont * font() const {
    return m_bufferTextView.font();
  }
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_bufferTextView.setAlignment(horizontalAlignment, verticalAlignment);
  }
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setText(const char * textContent);
  void setTextColor(KDColor textColor);
protected:
  static constexpr KDCoordinate k_horizontalMargin = Metric::CellMargin;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  BufferTextView m_bufferTextView;
};

#endif
