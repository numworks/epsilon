#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "app.h"
#include "calculation/discrete_calculation.h"
#include "calculation/left_integral_calculation.h"
#include "calculation/right_integral_calculation.h"
#include "calculation/finite_integral_calculation.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Probability {

CalculationController::ContentView::ContentView(Responder * parentResponder, CalculationController * calculationController, Calculation * calculation, Law * law) :
  m_titleView(KDText::FontSize::Small, I18n::Message::ComputeProbability, 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen),
  m_calculationView(parentResponder, calculationController, calculation, law),
  m_lawCurveView(law, calculation)
{
}

int CalculationController::ContentView::numberOfSubviews() const {
  return 3;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return &m_titleView;
  }
  if (index == 1) {
    return &m_lawCurveView;
  }
  return &m_calculationView;
}

void CalculationController::ContentView::layoutSubviews() {
  KDCoordinate titleHeight = KDText::charSize(KDText::FontSize::Small).height()+k_titleHeightMargin;
  m_titleView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  KDCoordinate calculationHeight = m_calculationView.minimalSizeForOptimalDisplay().height();
  m_calculationView.setFrame(KDRect(0,  titleHeight, bounds().width(), bounds().height()-titleHeight));
  m_lawCurveView.setFrame(KDRect(0,  titleHeight+calculationHeight, bounds().width(), bounds().height() - calculationHeight - titleHeight));
}

CalculationController::CalculationController(Responder * parentResponder, Law * law, Calculation * calculation) :
  ViewController(parentResponder),
  m_contentView(this, this, calculation, law),
  m_calculation(calculation),
  m_law(law)
{
  assert(law != nullptr);
  assert(calculation != nullptr);
}

View * CalculationController::view() {
  return &m_contentView;
}

const char * CalculationController::title() {
  return m_titleBuffer;
}

void CalculationController::reloadLawCurveView() {
  m_contentView.lawCurveView()->reload();
}

void CalculationController::reload() {
  m_contentView.calculationView()->reload();
  reloadLawCurveView();
}

void CalculationController::setCalculationAccordingToIndex(int index, bool forceReinitialisation) {
  if ((int)m_calculation->type() == index && !forceReinitialisation) {
    return;
  }
  m_calculation->~Calculation();
  switch (index) {
    case 0:
      new(m_calculation) LeftIntegralCalculation();
      break;
    case 1:
      new(m_calculation) FiniteIntegralCalculation();
      break;
    case 2:
      new(m_calculation) RightIntegralCalculation();
      break;
    case 3:
      new(m_calculation) DiscreteCalculation();
      break;
    default:
     return;
  }
  m_calculation->setLaw(m_law);
  reload();
}

void CalculationController::viewWillAppear() {
  reloadLawCurveView();
  m_contentView.calculationView()->selectSubview(1);
}

void CalculationController::didEnterResponderChain(Responder * previousResponder) {
  App::Snapshot * snapshot = (App::Snapshot *)app()->snapshot();
  snapshot->setActivePage(App::Snapshot::Page::Calculations);
  updateTitle();
}

void CalculationController::didBecomeFirstResponder() {
  app()->setFirstResponder(m_contentView.calculationView());
}

void CalculationController::updateTitle() {
  int currentChar = 0;
  for (int index = 0; index < m_law->numberOfParameter(); index++) {
    m_titleBuffer[currentChar++] = I18n::translate(m_law->parameterNameAtIndex(index))[0];
    strlcpy(m_titleBuffer+currentChar, " = ", 4);
    currentChar += 3;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
    Complex<double>::convertFloatToText(m_law->parameterValueAtIndex(index), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
    strlcpy(m_titleBuffer+currentChar, buffer, strlen(buffer)+1);
    currentChar += strlen(buffer);
    m_titleBuffer[currentChar++] = ' ';
  }
  m_titleBuffer[currentChar-1] = 0;
}

}
