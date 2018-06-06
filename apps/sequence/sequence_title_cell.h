#ifndef SEQUENCE_SEQUENCE_TITLE_CELL_H
#define SEQUENCE_SEQUENCE_TITLE_CELL_H

#include "../shared/function_title_cell.h"

namespace Sequence {

class SequenceTitleCell : public Shared::FunctionTitleCell {
public:
  SequenceTitleCell(Orientation orientation);
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  const char * text() const override {
    return m_storedText;
  }
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  EvenOddExpressionCell m_titleTextView;
  constexpr static int k_maxStoredTextSize = 8; // Worst case u_{n+1}
  char m_storedText[k_maxStoredTextSize];
};

}

#endif
