#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/expression_table_cell.h>
#include <escher/i18n.h>

namespace Escher {
class ExpressionTableCellWithExpression : public ExpressionTableCell {
 public:
  ExpressionTableCellWithExpression(Responder* parentResponder = nullptr,
                                    KDFont::Size font = KDFont::Size::Large);
  const View* subLabelView() const override {
    return &m_subLabelExpressionView;
  }
  void setHighlighted(bool highlight) override;
  void setSubLabelLayout(Poincare::Layout l);
  void setSubLabelFont(KDFont::Size font) {
    m_subLabelExpressionView.setFont(font);
  }
  void didBecomeFirstResponder() override;
  void reloadScroll() override { m_subLabelExpressionView.reloadScroll(); }

 private:
  // SubLabel margin is already handled in ScrollableExpressionView
  ScrollableExpressionView m_subLabelExpressionView;
};

}  // namespace Escher

#endif
