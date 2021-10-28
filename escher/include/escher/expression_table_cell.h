#ifndef ESCHER_EXPRESSION_TABLE_CELL_H
#define ESCHER_EXPRESSION_TABLE_CELL_H

#include <escher/scrollable_expression_view.h>
#include <escher/table_cell.h>

namespace Escher {

class ExpressionTableCell : public Responder, public TableCell {
public:
  ExpressionTableCell(Responder * responder = nullptr);
  const View * labelView() const override { return &m_labelExpressionView; }
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layout);
  void setTextColor(KDColor color) { m_labelExpressionView.setTextColor(color); }
  Poincare::Layout layout() const override { return m_labelExpressionView.layout(); }

  Responder * responder() override {
    return this;
  }
  void didBecomeFirstResponder() override;
  virtual void reloadScroll() { m_labelExpressionView.reloadScroll(); }
private:
  // Remove margins added by TableCell because they're already handled by ScrollableInputExactApproximateExpressionsView
  ScrollableExpressionView m_labelExpressionView;
};

}

#endif
