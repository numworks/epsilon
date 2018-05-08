#include "intersection_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

IntersectionGraphController::IntersectionGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, CartesianFunctionStore * store) :
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
  char buffer[FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * space = "                  ";
  int spaceLength = strlen(space);
  const char * legend = "0(x)=0(x)=";
  int legendLength = strlen(legend);
  int numberOfChar = 0;
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  buffer[0] = m_function->name()[0];
  buffer[5] = m_intersectedFunction->name()[0];
  numberOfChar += PrintFloat::convertFloatToText<double>(m_cursor->y(), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, spaceLength+1);
  buffer[FunctionBannerDelegate::k_maxDigitLegendLength+legendLength] = 0;
  bannerView()->setLegendAtIndex(buffer, 1);
}

CartesianFunction::Point IntersectionGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  CartesianFunction::Point result = {.abscissa = NAN, .value = NAN};
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    if (f != m_function) {
      CartesianFunction::Point intersection = m_function->nextIntersectionFrom(start, step, max, context, f);
      if ((std::isnan(result.abscissa) || std::fabs(intersection.abscissa-start) < std::fabs(result.abscissa-start)) && !std::isnan(intersection.abscissa)) {
        m_intersectedFunction = f;
        result = (std::isnan(result.abscissa) || std::fabs(intersection.abscissa-start) < std::fabs(result.abscissa-start)) ? intersection : result;
      }
    }
  }
  return result;
}

}
