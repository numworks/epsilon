#include "calculation_controller.h"
#include <assert.h>

namespace Probability {

CalculationController::ContentView::ContentView(Responder * parentResponder, Law * law) :
  m_lawCurveView(LawCurveView(law)),
  m_imageTableView(ImageTableView(parentResponder, law))
{
}

int CalculationController::ContentView::numberOfSubviews() const {
  return 2;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_lawCurveView;
  }
  return &m_imageTableView;
}

void CalculationController::ContentView::layoutSubviews() {
  m_lawCurveView.setFrame(KDRect(0,  ImageTableView::k_imageHeight, bounds().width(), bounds().height() - ImageTableView::k_imageHeight));
  m_imageTableView.setFrame(KDRect(0, 0, ImageTableView::k_imageWidth, 3*ImageTableView::k_imageHeight));
}

LawCurveView * CalculationController::ContentView::lawCurveView() {
  return &m_lawCurveView;
}

ImageTableView * CalculationController::ContentView::imageTableView() {
  return &m_imageTableView;
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
  if (event == Ion::Events::Left) {
    //m_selectableTableView.deselectTable();
    m_contentView.imageTableView()->select(true);
    app()->setFirstResponder(m_contentView.imageTableView());
    return true;
  }
 if (event == Ion::Events::Right) {
    m_contentView.imageTableView()->select(false);
    app()->setFirstResponder(this);
    return true;
  }
  return false;
}

void CalculationController::didBecomeFirstResponder() {
  m_contentView.lawCurveView()->reload();
}

}
