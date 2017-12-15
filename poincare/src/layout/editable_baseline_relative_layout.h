#ifndef POINCARE_EDITABLE_BASELINE_RELATIVE_LAYOUT_H
#define POINCARE_EDITABLE_BASELINE_RELATIVE_LAYOUT_H

#include "baseline_relative_layout.h"

namespace Poincare {

class EditableBaselineRelativeLayout : public BaselineRelativeLayout {
public:
  using BaselineRelativeLayout::BaselineRelativeLayout;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
};

}

#endif
