#ifndef POINCARE_CEILING_LAYOUT_H
#define POINCARE_CEILING_LAYOUT_H

#include "bracket_layout.h"
#include <poincare/layout_engine.h>

namespace Poincare {

class CeilingLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "ceil");
  }
protected:
  bool renderBottomBar() const override { return false; }
};

}

#endif
