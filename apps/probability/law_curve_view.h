#ifndef PROBABILITY_LAW_CURVE_VIEW_H
#define PROBABILITY_LAW_CURVE_VIEW_H

#include "../curve_view.h"
#include "../constant.h"
#include "law/law.h"
#include <escher.h>
#include <poincare.h>

namespace Probability {

class LawCurveView : public CurveView {
public:
  LawCurveView();
  void setLaw(Law * law);
  void reload();
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  float min(Axis axis) const override;
  float max(Axis axis) const override;
  float gridUnit(Axis axis) const override;
  char * label(Axis axis, int index) const override;
private:
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  float evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const override;
  Law * m_law;
};

}

#endif
