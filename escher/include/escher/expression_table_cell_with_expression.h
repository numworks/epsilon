#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/expression_table_cell.h>
#include <escher/i18n.h>

class ExpressionTableCellWithExpression : public ExpressionTableCell {
public:
  ExpressionTableCellWithExpression(Responder * parentResponder = nullptr);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryLayout(Poincare::Layout l);
  void didBecomeFirstResponder() override;
private:
  ScrollableExpressionView m_accessoryExpressionView;
};

#endif
