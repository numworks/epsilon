#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/expression_table_cell.h>
#include <escher/i18n.h>

namespace Escher {
// Cell : Varbox Calculation (expression)
class ExpressionTableCellWithExpression : public ExpressionTableCell {
public:
  ExpressionTableCellWithExpression(Responder * parentResponder = nullptr);
  View * subLabelView() const override; // Todoo sublabel
  void setHighlighted(bool highlight) override;
  void setSubLabelLayout(Poincare::Layout l);
  void didBecomeFirstResponder() override;
  void reloadScroll() override { m_subLabelExpressionView.reloadScroll(); }
private:
  // SubLabel margin is already handled in ScrollableExpressionView
  // KDCoordinate subLabelMargin() const override { return 0; }
  ScrollableExpressionView m_subLabelExpressionView;
};

}

#endif
