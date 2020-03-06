#ifndef ESCHER_EXPRESSION_TABLE_CELL_H
#define ESCHER_EXPRESSION_TABLE_CELL_H

#include <escher/scrollable_expression_view.h>
#include <escher/table_cell.h>

class ExpressionTableCell : public Responder, public TableCell {
public:
  ExpressionTableCell(Responder * responder = nullptr, Layout layout = Layout::HorizontalRightOverlap);
  View * labelView() const override;
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layout);
  Poincare::Layout layout() const override { return m_labelExpressionView.layout(); }

  Responder * responder() override {
    return this;
  }
  void didBecomeFirstResponder() override;
  virtual void reloadScroll() { m_labelExpressionView.reloadScroll(); }
private:
  // Remove margins added by TableCell because they're already handled by ScrollableInputExactApproximateExpressionsView
  KDCoordinate labelMargin() const override { return 0; }
  ScrollableExpressionView m_labelExpressionView;
};

#endif
