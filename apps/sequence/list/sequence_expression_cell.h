#ifndef SEQUENCE_SEQUENCE_EXPRESSION_CELL_H
#define SEQUENCE_SEQUENCE_EXPRESSION_CELL_H

#include "../sequence.h"
#include <escher.h>

namespace Sequence {

class SequenceExpressionCell : public Responder, public EvenOddCell {
public:
  SequenceExpressionCell(Responder * parentResponder);
  void setSequence(Sequence * sequence);
  int selectedSubCell();
  void selectSubCell(int index);
  void setHighlighted(bool highlight) override;
  void setEven(bool even) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  Sequence * m_sequence;
  int m_numberOfSubCells;
  int m_selectedSubCell;
  EvenOddExpressionCell m_expressionView;
  EvenOddExpressionCell m_firstInitialConditionView;
  EvenOddExpressionCell m_secondInitialConditionView;
};

}

#endif
