#ifndef ESCHER_POINTER_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H
#define ESCHER_POINTER_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H

#include <escher/pointer_table_cell_with_chevron.h>
#include <escher/expression_view.h>

class PointerTableCellWithChevronAndExpression : public PointerTableCellWithChevron {
public:
  PointerTableCellWithChevronAndExpression(char * label = nullptr);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
private:
  ExpressionView m_subtitleView;
};

#endif
