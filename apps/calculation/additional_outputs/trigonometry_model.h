#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H

#include "../../shared/curve_view_range.h"
#include <complex>
#include <poincare/trigonometry.h>

namespace Calculation {

class TrigonometryModel : public Shared::CurveViewRange {
public:
  TrigonometryModel();
  // CurveViewRange
  float xMin() const override { return -2.2f; }
  float xMax() const override { return 2.2f; }
  float yMin() const override { return std::sin(angle()) >= 0.0f ? -0.2f : -1.2f; }
  float yMax() const override { return std::sin(angle()) >= 0.0f ? 1.2f : 0.2f; }

  void setAngle(float f) { m_angle = f; }
  float angle() const { return m_angle*M_PI/Poincare::Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit()); }
private:
  float m_angle;
};

}

#endif
