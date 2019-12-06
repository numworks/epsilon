#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H

#include "../../shared/curve_view_range.h"
#include <complex>

namespace Calculation {

class TrigonometryModel : public Shared::CurveViewRange {
public:
  TrigonometryModel();
  // CurveViewRange
  float xMin() const override { return -1.5f; }
  float xMax() const override { return 1.5f; }
  float yMin() const override { return -1.5f; }
  float yMax() const override { return 1.5f; }

  void setAngle(float f) { m_angle = f; }
private:
  float m_angle;
};

}

#endif
