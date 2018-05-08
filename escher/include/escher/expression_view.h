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
  ExpressionView(float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  Poincare::ExpressionLayout * expressionLayout() const;
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  /* Warning: we do not need to delete the previous expression layout when
   * deleting object or setting a new expression layout. Indeed, the expression
   * layout is always possessed by a controller which only gives a pointer to
   * the expression view (without cloning it). The named controller is then
   * responsible for freeing the expression layout when required. */
  Poincare::ExpressionLayout * m_expressionLayout;
  float m_horizontalAlignment;
  float m_verticalAlignment;
  KDColor m_textColor;
  KDColor m_backgroundColor;
};

#endif
