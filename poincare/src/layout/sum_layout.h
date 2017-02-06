#ifndef POINCARE_SUM_LAYOUT_H
#define POINCARE_SUM_LAYOUT_H

#include "sequence_layout.h"

namespace Poincare {

class SumLayout : public SequenceLayout {
public:
  using SequenceLayout::SequenceLayout;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
