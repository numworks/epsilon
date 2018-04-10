#ifndef POINCARE_FLOOR_LAYOUT_H
#define POINCARE_FLOOR_LAYOUT_H

#include "bracket_layout.h"
#include <poincare/layout_engine.h>

namespace Poincare {

class FloorLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "floor");
  }
protected:
  bool renderTopBar() const override { return false; }
};

}

#endif
