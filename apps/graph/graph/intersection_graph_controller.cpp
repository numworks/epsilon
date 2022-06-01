#include "intersection_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <poincare/print.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

IntersectionGraphController::IntersectionGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoIntersectionFound)
{
}

const char * IntersectionGraphController::title() {
  return I18n::translate(I18n::Message::Intersection);
}

void IntersectionGraphController::reloadBannerView() {
  assert(!m_intersectedRecord.isNull());
  CalculationGraphController::reloadBannerView();
  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  const char * legend = "=";
  // 'f(x)=g(x)=', keep 2 chars for '='
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(m_record);
  size_t numberOfChar = f->nameWithArgument(buffer, bufferSize - 2 * strlen(legend));
  assert(numberOfChar < bufferSize);
  numberOfChar += Poincare::Print::customPrintf(buffer + numberOfChar, bufferSize - numberOfChar, legend);
  // keep 1 char for '=';
  ExpiringPointer<ContinuousFunction> g = functionStore()->modelForRecord(m_intersectedRecord);
  numberOfChar += g->nameWithArgument(buffer+numberOfChar, bufferSize - numberOfChar - strlen(legend));
  assert(numberOfChar <= bufferSize);
  Poincare::Print::customPrintf(buffer + numberOfChar, bufferSize - numberOfChar, "%s%*.*ed",
    legend,
    m_cursor->y(), Poincare::Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits());
  bannerView()->ordinateView()->setText(buffer);
  bannerView()->reload();
}

Poincare::Coordinate2D<double> IntersectionGraphController::computeNewPointOfInterest(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) {
  // TODO The following three lines should be factored.
  Poincare::Coordinate2D<double> result = Poincare::Coordinate2D<double>(NAN, NAN);
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    if (record != m_record) {
      ContinuousFunction f = *(functionStore()->modelForRecord(record));
      if (!f.isIntersectable()) {
        continue;
      }
      Poincare::Coordinate2D<double> intersection = functionStore()->modelForRecord(m_record)->nextIntersectionFrom(start, max, context, f.expressionReduced(context), relativePrecision, minimalStep, maximalStep, f.tMin(), f.tMax());
      if ((std::isnan(result.x1()) || std::fabs(intersection.x1()-start) < std::fabs(result.x1()-start)) && !std::isnan(intersection.x1())) {
        m_intersectedRecord = record;
        result = (std::isnan(result.x1()) || std::fabs(intersection.x1()-start) < std::fabs(result.x1()-start)) ? intersection : result;
      }
    }
  }
  return result;
}

}
