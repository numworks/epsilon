#ifndef PROBABILITY_LAW_CURVE_VIEW_H
#define PROBABILITY_LAW_CURVE_VIEW_H

#include "../curve_view.h"
#include "law.h"
#include <escher.h>
#include <poincare.h>

namespace Probability {

class LawCurveView : public CurveView {
public:
  LawCurveView(Law * law);
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  float min(Axis axis) const override;
  float max(Axis axis) const override;
private:
  float evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const override;
  Law * m_law;
};

}

#endif
