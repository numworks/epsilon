#ifndef GRAPH_FUNCTION_TITLE_CELL_H
#define GRAPH_FUNCTION_TITLE_CELL_H

#include "../shared/function_title_cell.h"

namespace Graph {

class FunctionTitleCell : public Shared::FunctionTitleCell {
public:
  FunctionTitleCell(Orientation orientation, KDText::FontSize size = KDText::FontSize::Large);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  EvenOddBufferTextCell m_bufferTextView;
};

}

#endif
