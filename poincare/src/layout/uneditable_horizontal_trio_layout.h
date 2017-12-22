#ifndef POINCARE_UNEDITABLE_HORIZONTAL_TRIO_LAYOUT_H
#define POINCARE_UNEDITABLE_HORIZONTAL_TRIO_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

/*  UneditableHorizontalTrioLayout has 3 children: a left and a right layout
 *  (usually parentheses or brackets), and a central layout.
 *  The cursor can only be:
 *  - Left or Right of the UneditableHorizontalTrioLayout,
 *  - Left or Right of the central layout if it is not an horizontal layout,
 *  - Inside the central layout if it is an horizontal layout.
 * This way, the lateral children of an UneditableHorizontalTrioLayout cannot be
 * edited, and it it will always have only 3 children.
 * This layout can be used to create binomial coefficient layouts, matrix
 * layouts or the argument of sum and product layouts. */

class UneditableHorizontalTrioLayout : public StaticLayoutHierarchy<3> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  ExpressionLayout * leftLayout();
  ExpressionLayout * centerLayout();
  ExpressionLayout * rightLayout();
};

}

#endif
