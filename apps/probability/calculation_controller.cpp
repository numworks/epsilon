#include "calculation_controller.h"
#include <assert.h>

namespace Probability {

CalculationController::ContentView::ContentView(Responder * parentResponder, Law * law) :
  m_lawCurveView(LawCurveView(law))
{
}

int CalculationController::ContentView::numberOfSubviews() const {
  return 1;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_lawCurveView;
}

void CalculationController::ContentView::layoutSubviews() {
  m_lawCurveView.setFrame(bounds());
}

LawCurveView * CalculationController::ContentView::lawCurveView() {
  return &m_lawCurveView;
}

CalculationController::CalculationController(Responder * parentResponder, Law * law) :
  ViewController(parentResponder),
  m_contentView(ContentView(this, law)),
  m_law(law)
{
}

View * CalculationController::view() {
  return &m_contentView;
}

const char * CalculationController::title() const {
  return "Calculer les probabilites";
}

bool CalculationController::handleEvent(Ion::Events::Event event) {
  return false;
}

void CalculationController::didBecomeFirstResponder() {
  m_contentView.lawCurveView()->reload();
}

}
