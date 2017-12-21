#ifndef POINCARE_UNEDITABLE_PARENTHESIS_RIGHT_LAYOUT_H
#define POINCARE_UNEDITABLE_PARENTHESIS_RIGHT_LAYOUT_H

#include <poincare/src/layout/parenthesis_right_layout.h>

namespace Poincare {

class UneditableParenthesisRightLayout : public ParenthesisRightLayout {
public:
  using ParenthesisRightLayout::ParenthesisRightLayout;
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
};

}

#endif
