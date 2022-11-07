#include "function_model.h"
#include "../app.h"
#include <apps/shared/function.h>
#include <poincare/zoom.h>
#include <algorithm>

using namespace Poincare;

namespace Calculation {

static Context * context() { return App::app()->localContext(); }

void FunctionModel::setParameters(Poincare::Expression function, float abscissa) {
  m_function = function;
  m_abscissa = abscissa;
  m_ordinate = Shared::PoincareHelpers::ApproximateWithValueForSymbol<float>(m_function, Shared::Function::k_unknownName, abscissa, context());
  recomputeViewRange();
};

float FunctionModel::RangeMargin(bool maxMargin, float rangeBound, float value, float pixelRatio) {
  float currentMargin = (maxMargin ? (rangeBound - std::max(value, 0.0f)) : (std::min(value, 0.0f) - rangeBound)) / pixelRatio;
  return currentMargin >= k_marginInPixels ? 0.0 : k_marginInPixels * pixelRatio;
}

void FunctionModel::recomputeViewRange() {
  Zoom zoom(-1 * Range1D::k_maxFloat, Range1D::k_maxFloat, 1 / k_xyRatio, context());

  Zoom::Function2DWithContext evaluator = [](float t, const void * model, Context * context) {
    const Expression * f = static_cast<const Expression *>(model);
    return Coordinate2D<float>(t, Shared::PoincareHelpers::ApproximateWithValueForSymbol<float>(*f, Shared::Function::k_unknownName, t, context));
  };
  zoom.fitPointsOfInterest(evaluator, static_cast<void *>(&m_function));

  zoom.fitPoint(Coordinate2D<float>(m_abscissa, m_ordinate));
  zoom.fitPoint(Coordinate2D<float>(0.0f, 0.0f));

  Range2D range = zoom.range(Range1D::k_maxFloat, false);

  float widthPixelRatio =  range.x()->length() / k_width;
  float heigthPixelRatio =  range.y()->length() / k_height;

  setXMin(range.xMin() - RangeMargin(false, range.xMin(), m_abscissa, widthPixelRatio));
  setXMax(range.xMax() + RangeMargin(true, range.xMax(), m_abscissa, widthPixelRatio));
  setYMin(range.yMin() - RangeMargin(false, range.yMin(), m_ordinate, heigthPixelRatio));
  setYMax(range.yMax() + RangeMargin(true, range.yMax(), m_ordinate, heigthPixelRatio));
}

}
