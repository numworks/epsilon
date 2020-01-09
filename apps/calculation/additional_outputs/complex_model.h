#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_MODEL_H

#include "../../shared/curve_view_range.h"
#include <complex>

namespace Calculation {

class ComplexModel : public Shared::CurveViewRange, public std::complex<float> {
public:
  ComplexModel(std::complex<float> c = std::complex<float>(NAN, NAN));
  // CurveViewRange
  float xMin() const override;
  float xMax() const override;
  float yMin() const override;
  float yMax() const override;

  void setComplex(std::complex<float> c) { *this = ComplexModel(c); }


  static constexpr float k_minVerticalMarginFactor = -0.5f;
  static constexpr float k_maxVerticalMarginFactor = 1.2f;
  static constexpr float k_minHorizontalMarginFactor = -1.0f;
  static constexpr float k_maxHorizontalMarginFactor = 2.0f;

private:
  float rangeBound(float direction, bool horizontal) const;
};

}

#endif
