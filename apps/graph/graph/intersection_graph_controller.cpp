#include "intersection_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <poincare/print.h>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

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
  size_t fNameLength = numberOfChar;
  numberOfChar += Print::CustomPrintf(buffer + numberOfChar, bufferSize - numberOfChar, legend);
  // keep 1 char for '=';
  ExpiringPointer<ContinuousFunction> g = functionStore()->modelForRecord(m_intersectedRecord);
  size_t numberOfCharsBeforeGName = numberOfChar;
  numberOfChar += g->nameWithArgument(buffer+numberOfChar, bufferSize - numberOfChar - strlen(legend));
  if (numberOfChar-numberOfCharsBeforeGName==fNameLength && memcmp(buffer, buffer+numberOfCharsBeforeGName, fNameLength)==0) {
    numberOfChar = fNameLength;
  }
  assert(numberOfChar <= bufferSize);
  Print::CustomPrintf(buffer + numberOfChar, bufferSize - numberOfChar, "%s%*.*ed",
    legend,
    m_cursor->y(), Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits());
  bannerView()->ordinateView()->setText(buffer);
  bannerView()->reload();
}

Coordinate2D<double> IntersectionGraphController::computeNewPointOfInterest(double start, double max, Context * context) {
  Coordinate2D<double> result = Coordinate2D<double>(NAN, NAN);
  int numberOfActiveFunctions = functionStore()->numberOfActiveFunctions();
  for (int i = 0; i < numberOfActiveFunctions; i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    if (record == m_record) {
      continue;
    }
    ContinuousFunction f = *(functionStore()->modelForRecord(record));
    if (!f.isIntersectable()) {
      continue;
    }

    double fStart = start, fEnd = max;
    f.trimResolutionInterval(&fStart, &fEnd);
    Solver<double> solver = PoincareHelpers::Solver(fStart, fEnd, ContinuousFunction::k_unknownName, context);
    Coordinate2D<double> intersection = solver.nextIntersection(f.expressionClone(), functionStore()->modelForRecord(m_record)->expressionClone());
    if (std::isnan(intersection.x1())) {
      continue;
    }

    if (std::isnan(result.x1()) || (std::fabs(intersection.x1() - start) < std::fabs(result.x1() - start))) {
      m_intersectedRecord = record;
      result = intersection;
    }
  }

  return result;
}

}
