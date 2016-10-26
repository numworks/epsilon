#ifndef ESCHER_EXPRESSION_VIEW_H
#define ESCHER_EXPRESSION_VIEW_H

#include <escher/view.h>
#include <kandinsky/color.h>
#include <poincare/expression_layout.h>

/* This class does not handle the expression layout as the size of the layout is
 * needed to compute the size of table cells hosting the expression. As the size
 * of this cell is determined before we set the expression in the expression
 * view, we cannot use minimalSizeForOptimalDisplay to assess the required
 * size. */

class ExpressionView : public View {
public:
  ExpressionView();
  void setExpression(ExpressionLayout * expressionLayout);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  KDSize minimalSizeForOptimalDisplay() override;
private:
  ExpressionLayout * m_expressionLayout;
  KDColor m_textColor;
  KDColor m_backgroundColor;
};

#endif
