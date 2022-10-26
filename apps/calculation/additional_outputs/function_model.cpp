#include "function_model.h"
#include "../app.h"
#include <algorithm>

namespace Calculation {

void FunctionModel::setParameters(Poincare::Expression function, float abscissa) {
  m_function = function;
  m_abscissa = abscissa;
  constexpr static char k_unknownName[2] = {UCodePointUnknown, 0};
  m_ordinate = Shared::PoincareHelpers::ApproximateWithValueForSymbol<float>(m_function, k_unknownName, abscissa, App::app()->localContext());
  recomputeViewRange();
};

void FunctionModel::recomputeViewRange() {
  // TODO: to be replaced using the new zoom API
  const float x = m_abscissa * 2;
  const float y = m_ordinate * 2;

  float width = std::abs(x);
  float height = std::abs(y);
  bool verticallyCapped = height * k_xyRatio > width;
  float ratio = verticallyCapped ? height/k_height : width/k_width;

  float xMargin = k_marginInPixels*ratio + (verticallyCapped ? (height * k_xyRatio - width) / 2.f : 0.f);
  float yMargin = k_marginInPixels*ratio + (verticallyCapped ? 0.f : (width / k_xyRatio - height) / 2.f);

  setXMin(std::min(0.f, x) - xMargin);
  setXMax(std::max(0.f, x) + xMargin);
  setYMin(std::min(0.f, y) - yMargin);
  setYMax(std::max(0.f, y) + yMargin);
}

}
