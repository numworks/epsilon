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
  float rightMargin = (range.xMax() - std::max(m_abscissa, 0.0f)) / widthPixelRatio;
  float leftMargin = (std::min(m_abscissa, 0.0f) - range.xMin()) / widthPixelRatio;

  setXMin(range.xMin() - (leftMargin >= k_marginInPixels ? 0.0 : k_marginInPixels * widthPixelRatio));
  setXMax(range.xMax() + (rightMargin >= k_marginInPixels ? 0.0 : k_marginInPixels * widthPixelRatio));

  float heigthPixelRatio =  range.y()->length() / k_height;
  float topMargin = (range.yMax() - std::max(m_ordinate, 0.0f)) / heigthPixelRatio;
  float bottomMargin = (std::min(m_ordinate, 0.0f) - range.yMin()) / heigthPixelRatio;

  setYMin(range.yMin() - (bottomMargin >= k_marginInPixels ? 0.0 : k_marginInPixels * heigthPixelRatio));
  setYMax(range.yMax() + (topMargin >= k_marginInPixels ? 0.0 : k_marginInPixels * heigthPixelRatio));
}

}
