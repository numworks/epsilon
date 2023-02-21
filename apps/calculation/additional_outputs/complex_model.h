#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_MODEL_H

#include <complex>

#include "../../shared/curve_view_range.h"
#include "illustrated_expressions_list_controller.h"

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

  /* The range is computed from these criteria:
   * - The real part is centered horizontally
   * - Both left and right margins are equal to the real length
   * - The imaginary part is the same length as the real part
   * - The remaining vertical margin are splitted as one third at the top, 2
   *   thirds at the bottom
   *
   *           |                        | 1/3 * vertical_margin
   *           +----------+
   *           |        / |             |
   *           |      /   |             | Imaginary
   *           |    /     |             |
   *           |  /       |             |
   * ----------+----------+----------
   *                                    |
   *                                    | 2/3 * vertical_margin
   *           -----------
   *              Real
   *
   */
  // Horizontal range
  constexpr static float k_minHorizontalMarginFactor = -1.0f;
  constexpr static float k_maxHorizontalMarginFactor = 2.0f;
  // Vertical range
  constexpr static KDCoordinate k_width = Ion::Display::Width -
                                          Escher::Metric::PopUpRightMargin -
                                          Escher::Metric::PopUpLeftMargin;
  constexpr static KDCoordinate k_height =
      IllustratedExpressionsListController::k_illustrationHeight;
  constexpr static KDCoordinate k_unit = k_width / 3;
  /*
   *  VerticalMaring = k_height - k_unit
   *
   *  Values | Coordinates
   * --------+----------------------------------
   *  imag   |  k_unit
   *  Ymax   |  k_unit + (1/3)*VerticalMargin
   *  Ymin   |  -(2/3)*VerticalMargin
   *
   * Thus:
   * Ymin = -(2/3)*k_verticalMargin*imag/k_unit
   *      = -(2/3)*(k_height/k_unit - 1)*imag
   *      = 2/3*(1 - k_height/k_unit)*imag
   * Ymax = (k_unit + (1/3)*VerticalMargin)*imag/k_unit
   *      = (1 + (1/3)*(k_height/k_unit - 1))*imag
   *      = 1/3*(2 + k_height/k_unit)*imag
   *
   * */
  constexpr static float k_minVerticalMarginFactor =
      2.0f / 3.0f * (1.0f - (float)k_height / (float)k_unit);
  constexpr static float k_maxVerticalMarginFactor =
      1.0f / 3.0f * (2.0f + (float)k_height / (float)k_unit);

 private:
  float rangeBound(float direction, bool horizontal) const;
};

}  // namespace Calculation

#endif
