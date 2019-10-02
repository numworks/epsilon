#ifndef SHARED_BUFFER_FUNCTION_TITLE_CELL_H
#define SHARED_BUFFER_FUNCTION_TITLE_CELL_H

#include "function_title_cell.h"

namespace Shared {

class BufferFunctionTitleCell : public FunctionTitleCell {
public:
  BufferFunctionTitleCell(Orientation orientation = Orientation::VerticalIndicator, const KDFont * font = KDFont::LargeFont);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  void setText(const char * textContent);
  void setFont(const KDFont * font) { m_bufferTextView.setFont(font); }
  void setHorizontalAlignment(const float alignment) { return m_bufferTextView.setAlignment(alignment, k_verticalAlignment); }
  const KDFont * font() const override { return m_bufferTextView.font(); }
  const char * text() const override {
    return m_bufferTextView.text();
  }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
protected:
  KDRect bufferTextViewFrame() const;
  EvenOddBufferTextCell * bufferTextView() { return &m_bufferTextView; }
private:
  constexpr static float k_verticalAlignment = 0.5f;
  EvenOddBufferTextCell m_bufferTextView;
};

}

#endif
