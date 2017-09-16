#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class AbsoluteValueLayout : public BracketLayout {
public:
  AbsoluteValueLayout(ExpressionLayout * operandLayout) : BracketLayout(operandLayout) {}
  ~AbsoluteValueLayout() {}
  AbsoluteValueLayout(const AbsoluteValueLayout& other) = delete;
  AbsoluteValueLayout(AbsoluteValueLayout&& other) = delete;
  AbsoluteValueLayout& operator=(const AbsoluteValueLayout& other) = delete;
  AbsoluteValueLayout& operator=(AbsoluteValueLayout&& other) = delete;
protected:
  KDCoordinate widthMargin() const { return 2; }
  bool renderTopBar() const { return false; }
  bool renderBottomBar() const { return false; }
};

}

#endif
