#ifndef POINCARE_EMPTY_LAYOUT_H
#define POINCARE_EMPTY_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <assert.h>

namespace Poincare {

class EmptyLayout : public StaticLayoutHierarchy<0> {
public:
  EmptyLayout();
  ExpressionLayout * clone() const override;
  void addBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother) override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override { return; }
  virtual KDSize computeSize() override { return KDSizeZero; }
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override {
    assert(false);
    return KDPointZero;
  }
};

}

#endif
