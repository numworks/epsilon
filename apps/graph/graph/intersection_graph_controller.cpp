#include "intersection_graph_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"

using namespace Shared;

namespace Graph {

IntersectionGraphController::IntersectionGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, StorageCartesianFunctionStore * store) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoIntersectionFound),
  m_intersectedFunction(nullptr),
  m_functionStore(store)
{
}

const char * IntersectionGraphController::title() {
  return I18n::translate(I18n::Message::Intersection);
}

void IntersectionGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(2);
  reloadBannerViewForCursorOnFunction(m_cursor, m_function, 'x');
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = "                  ";
  const char * legend = "=";
  // 'f(x)=g(x)=', keep 2 chars for '='
  int numberOfChar = m_function->nameWithArgument(buffer, bufferSize-2, 'x');
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  // keep 1 char for '=';
  numberOfChar += m_intersectedFunction->nameWithArgument(buffer, bufferSize-numberOfChar-1, 'x');
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(m_cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->setLegendAtIndex(buffer, 1);
}

Poincare::Expression::Coordinate2D IntersectionGraphController::computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) {
  Poincare::Expression::Coordinate2D result = {.abscissa = NAN, .value = NAN};
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    StorageFunction * f = m_functionStore->activeFunctionAtIndex(i);
    if (f != m_function) {
      Poincare::Expression::Coordinate2D intersection = m_function->nextIntersectionFrom(start, step, max, context, f);
      if ((std::isnan(result.abscissa) || std::fabs(intersection.abscissa-start) < std::fabs(result.abscissa-start)) && !std::isnan(intersection.abscissa)) {
        m_intersectedFunction = f;
        result = (std::isnan(result.abscissa) || std::fabs(intersection.abscissa-start) < std::fabs(result.abscissa-start)) ? intersection : result;
      }
    }
  }
  return result;
}

}
