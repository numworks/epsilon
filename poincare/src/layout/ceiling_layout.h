#ifndef POINCARE_CEILING_LAYOUT_H
#define POINCARE_CEILING_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class CeilingLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  ExpressionLayout * clone() const override;
protected:
  bool renderBottomBar() const override { return false; }
};

}

#endif
