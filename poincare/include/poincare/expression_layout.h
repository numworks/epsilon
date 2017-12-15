#ifndef POINCARE_EXPRESSION_LAYOUT_H
#define POINCARE_EXPRESSION_LAYOUT_H

#include <kandinsky.h>

namespace Poincare {

class ExpressionLayoutCursor;

class ExpressionLayout {
public:
  ExpressionLayout();
  virtual  ~ExpressionLayout() = default;

  /* Rendering */
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  KDPoint origin();
  KDPoint absoluteOrigin();
  KDSize size();
  KDCoordinate baseline();

  /* Hierarchy */
  void setParent(ExpressionLayout* parent);

  /* Tree navigation */
  virtual bool moveLeft(ExpressionLayoutCursor * cursor) { return false; } //TODO should be virtual pure?
protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
  virtual KDSize computeSize() = 0;
  virtual ExpressionLayout * child(uint16_t index) = 0;
  virtual KDPoint positionOfChild(ExpressionLayout * child) = 0;
  KDCoordinate m_baseline;
  ExpressionLayout * m_parent;
private:
  bool m_sized, m_positioned;
  KDRect m_frame;
};

}

#endif
