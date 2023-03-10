#ifndef APPS_SHARED_EXPRESSION_FUNCTION_TITLE_CELL_H
#define APPS_SHARED_EXPRESSION_FUNCTION_TITLE_CELL_H

#include <escher/even_odd_expression_cell.h>
#include <poincare/code_point_layout.h>

#include "function_title_cell.h"

namespace Shared {

class ExpressionFunctionTitleCell : public FunctionTitleCell {
 public:
  ExpressionFunctionTitleCell(KDFont::Size font = KDFont::Size::Small);
  void setLayout(Poincare::Layout layout) { m_titleTextView.setLayout(layout); }
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  Poincare::Layout layout() const override { return m_titleTextView.layout(); }
  void reloadCell() override;

 protected:
  void layoutSubviews(bool force = false) override;

  Escher::EvenOddExpressionCell m_titleTextView;

 private:
  constexpr static float k_alignment = KDGlyph::k_alignCenter;

  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override;
};

}  // namespace Shared

#endif
