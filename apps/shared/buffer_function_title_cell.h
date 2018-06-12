#ifndef SHARED_BUFFER_FUNCTION_TITLE_CELL_H
#define SHARED_BUFFER_FUNCTION_TITLE_CELL_H

#include "function_title_cell.h"

namespace Shared {

class BufferFunctionTitleCell : public FunctionTitleCell {
public:
  BufferFunctionTitleCell(Orientation orientation, KDText::FontSize size = KDText::FontSize::Large);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  void setText(const char * textContent);
  const char * text() const override {
    return m_bufferTextView.text();
  }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  KDRect bufferTextViewFrame() const;
  EvenOddBufferTextCell * bufferTextView() { return &m_bufferTextView; }
private:
  EvenOddBufferTextCell m_bufferTextView;
};

}

#endif
