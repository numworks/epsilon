#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class AbsoluteValueLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  ExpressionLayout * clone() const override;
protected:
  KDCoordinate widthMargin() const override { return 2; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

}

#endif
