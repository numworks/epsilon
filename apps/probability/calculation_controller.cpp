#include "calculation_controller.h"
#include <assert.h>

namespace Probability {

CalculationController::ContentView::ContentView(Responder * parentResponder, Law * law) :
  m_lawCurveView(LawCurveView(law)),
  m_imageTableView(ImageTableView(parentResponder, law))
{
}

int CalculationController::ContentView::numberOfSubviews() const {
  return 6;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 6);
  if (index == 0) {
    return &m_lawCurveView;
  }
  if (index == 1) {
    return &m_imageTableView;
  }
  if (index == 2) {
    m_text[0].setAlignment(0.5f, 0.5f);
    m_text[0].setText("P(X <=");
    return &m_text[0];
  }
  if (index == 3) {
    m_calculationCell[0].setText("test");
    return &m_calculationCell[0];
  }
  if (index == 4) {
    m_text[1].setAlignment(0.5f, 0.5f);
    m_text[1].setText(") = ");
    return &m_text[1];
  }
  if (index == 5) {
    m_calculationCell[1].setText("test2");
    return &m_calculationCell[1];
  }
  return nullptr;
}

void CalculationController::ContentView::layoutSubviews() {
  KDCoordinate xCoordinate = 0;
  m_lawCurveView.setFrame(KDRect(0,  ImageTableView::k_imageHeight, bounds().width(), bounds().height() - ImageTableView::k_imageHeight));
  m_imageTableView.setFrame(KDRect(xCoordinate, 0, ImageTableView::k_imageWidth, 3*ImageTableView::k_imageHeight));
  xCoordinate += ImageTableView::k_imageWidth + k_textMargin;
  m_text[0].setFrame(KDRect(xCoordinate, 0, 7*k_charWidth, ImageTableView::k_imageHeight));
  xCoordinate += 7*k_charWidth + k_textMargin;
  m_calculationCell[0].setFrame(KDRect(xCoordinate, 0, k_textFieldWidth, ImageTableView::k_imageHeight));
  xCoordinate += k_textFieldWidth + k_textMargin;
  m_text[1].setFrame(KDRect(xCoordinate, 0, 7*k_charWidth, ImageTableView::k_imageHeight));
  xCoordinate += 7*k_charWidth + k_textMargin;
  m_calculationCell[1].setFrame(KDRect(xCoordinate, 0, k_textFieldWidth, ImageTableView::k_imageHeight));
}

void CalculationController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
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
