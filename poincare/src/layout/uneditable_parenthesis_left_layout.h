#ifndef POINCARE_UNEDITABLE_PARENTHESIS_LEFT_LAYOUT_H
#define POINCARE_UNEDITABLE_PARENTHESIS_LEFT_LAYOUT_H

#include <poincare/src/layout/parenthesis_left_layout.h>

namespace Poincare {

class UneditableParenthesisLeftLayout : public ParenthesisLeftLayout {
public:
  using ParenthesisLeftLayout::ParenthesisLeftLayout;
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
};

}

#endif
