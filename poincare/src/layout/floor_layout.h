#ifndef POINCARE_FLOOR_LAYOUT_H
#define POINCARE_FLOOR_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class FloorLayout : public BracketLayout {
public:
  FloorLayout(ExpressionLayout * operandLayout) : BracketLayout(operandLayout) {}
  ~FloorLayout() {}
  FloorLayout(const FloorLayout& other) = delete;
  FloorLayout(FloorLayout&& other) = delete;
  FloorLayout& operator=(const FloorLayout& other) = delete;
  FloorLayout& operator=(FloorLayout&& other) = delete;
protected:
  bool renderTopBar() const override { return false; }
};

}

#endif
