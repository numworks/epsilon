#ifndef POINCARE_EDITABLE_BASELINE_RELATIVE_LAYOUT_H
#define POINCARE_EDITABLE_BASELINE_RELATIVE_LAYOUT_H

#include "baseline_relative_layout.h"

namespace Poincare {

class EditableBaselineRelativeLayout : public BaselineRelativeLayout {
public:
  using BaselineRelativeLayout::BaselineRelativeLayout;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
};

}

#endif
