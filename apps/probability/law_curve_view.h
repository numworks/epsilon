#ifndef PROBABILITY_LAW_CURVE_VIEW_H
#define PROBABILITY_LAW_CURVE_VIEW_H

#include "../shared/curve_view.h"
#include "../constant.h"
#include "law/law.h"
#include "calculation/calculation.h"
#include <escher.h>
#include <poincare.h>

namespace Probability {

class LawCurveView : public Shared::CurveView {
public:
  LawCurveView(Law * law, Calculation * calculation);
  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  char * label(Axis axis, int index) const override;
private:
  char m_labels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  static float EvaluateAtAbscissa(float abscissa, void * model, void * context);
  Law * m_law;
  Calculation * m_calculation;
};

}

#endif
