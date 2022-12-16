#include "function_model.h"
#include "../app.h"
#include <apps/shared/function.h>
#include <apps/shared/interactive_curve_view_range.h>
#include <poincare/zoom.h>
#include <algorithm>

using namespace Poincare;

namespace Calculation {

static Context * context() { return App::app()->localContext(); }

void FunctionModel::setParameters(Poincare::Expression function, float abscissa, float ordinate) {
  m_function = function;
  m_abscissa = abscissa;
  m_ordinate = ordinate;
  recomputeViewRange();
};

float FunctionModel::RangeMargin(bool maxMargin, float rangeBound, float value, float pixelRatio) {
  float currentMargin = (maxMargin ? (rangeBound - std::max(value, 0.0f)) : (std::min(value, 0.0f) - rangeBound)) / pixelRatio;
  return currentMargin >= k_marginInPixels ? 0.0 : k_marginInPixels * pixelRatio;
}

template<typename T> static Coordinate2D<T> evaluator(T t, const void * model, Context * context) {
  const Expression * f = static_cast<const Expression *>(model);
  return Coordinate2D<T>(t, Shared::PoincareHelpers::ApproximateWithValueForSymbol<T>(*f, Shared::Function::k_unknownName, t, context));
}

void FunctionModel::recomputeViewRange() {
  constexpr float k_maxFloat = Shared::InteractiveCurveViewRange::k_maxFloat;
  Zoom zoom(-k_maxFloat, k_maxFloat, 1 / k_xyRatio, context(), k_maxFloat);

  zoom.fitPointsOfInterest(evaluator<float>, &m_function, false, evaluator<double>);

  zoom.fitPoint(Coordinate2D<float>(m_abscissa, m_ordinate));
  zoom.fitPoint(Coordinate2D<float>(0.0f, 0.0f));

  Range2D range = zoom.range(true, false);

  float widthPixelRatio =  range.x()->length() / k_width;
  float heigthPixelRatio =  range.y()->length() / k_height;

  setXMin(range.xMin() - RangeMargin(false, range.xMin(), m_abscissa, widthPixelRatio));
  setXMax(range.xMax() + RangeMargin(true, range.xMax(), m_abscissa, widthPixelRatio));
  setYMin(range.yMin() - RangeMargin(false, range.yMin(), m_ordinate, heigthPixelRatio));
  setYMax(range.yMax() + RangeMargin(true, range.yMax(), m_ordinate, heigthPixelRatio));
}

}
