#ifndef POINCARE_CEILING_LAYOUT_H
#define POINCARE_CEILING_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class CeilingLayout : public BracketLayout {
public:
  CeilingLayout(ExpressionLayout * operandLayout) : BracketLayout(operandLayout) {}
  ~CeilingLayout() {}
  CeilingLayout(const CeilingLayout& other) = delete;
  CeilingLayout(CeilingLayout&& other) = delete;
  CeilingLayout& operator=(const CeilingLayout& other) = delete;
  CeilingLayout& operator=(CeilingLayout&& other) = delete;
protected:
  bool renderBottomBar() const override { return false; }
};

}

#endif
