#ifndef POINCARE_FLOOR_LAYOUT_H
#define POINCARE_FLOOR_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class FloorLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  ExpressionLayout * clone() const override;
protected:
  bool renderTopBar() const override { return false; }
};

}

#endif
