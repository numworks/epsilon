#ifndef POINCARE_EXPRESSION_LAYOUT_H
#define POINCARE_EXPRESSION_LAYOUT_H

#include <kandinsky.h>

namespace Poincare {

class ExpressionLayout {
public:
  ExpressionLayout();
  virtual  ~ExpressionLayout() = default;

  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  KDPoint origin();
  KDSize size();
  KDCoordinate baseline();
  void setParent(ExpressionLayout* parent);
protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
  virtual KDSize computeSize() = 0;
  virtual ExpressionLayout * child(uint16_t index) = 0;
  virtual KDPoint positionOfChild(ExpressionLayout * child) = 0;
  KDCoordinate m_baseline;
private:
  KDPoint absoluteOrigin();
  //void computeLayout();//ExpressionLayout * parent, uint16_t childIndex);
  ExpressionLayout* m_parent;
  bool m_sized, m_positioned;
  KDRect m_frame;
};

}

#endif
