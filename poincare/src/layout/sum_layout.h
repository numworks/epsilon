#ifndef POINCARE_SUM_LAYOUT_H
#define POINCARE_SUM_LAYOUT_H

#include "sequence_layout.h"
#include <poincare/layout_engine.h>

namespace Poincare {

class SumLayout final : public SequenceLayout {
public:
  using SequenceLayout::SequenceLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
