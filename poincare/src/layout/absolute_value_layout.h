#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_H

#include "bracket_pair_layout.h"
#include <poincare/layout_engine.h>
#include <poincare/print_float.h>

namespace Poincare {

class AbsoluteValueLayout : public BracketPairLayout {
public:
  using BracketPairLayout::BracketPairLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "abs");
  }
protected:
  KDCoordinate widthMargin() const override { return 2; }
  virtual KDCoordinate verticalExternMargin() const override { return 1; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

}

#endif
