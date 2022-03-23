#include "box_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, ButtonRowController * header, Responder * tabController, Escher::StackViewController * stackViewController, Escher::ViewController * typeViewController, Store * store, int * selectedBoxCalculation, int * selectedSeriesIndex) :
  MultipleDataViewController(parentResponder, tabController, header, stackViewController, typeViewController, store, (int *)(selectedBoxCalculation), selectedSeriesIndex),
  m_view(store, selectedBoxCalculation),
  m_boxParameterController(nullptr, store),
  m_parameterButton(this, I18n::Message::StatisticsGraphSettings, Invocation([](void * context, void * sender) {
    BoxController * boxController = static_cast<BoxController * >(context);
    boxController->stackController()->push(boxController->boxParameterController());
    return true;
  }, this), KDFont::SmallFont)
{
}

Button * BoxController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  return index == 0 ? GraphButtonRowDelegate::buttonAtIndex(index, position) : const_cast<Button *>(&m_parameterButton);
}

bool BoxController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Toolbox) {
    stackController()->push(boxParameterController());
    return true;
  }
  return MultipleDataViewController::handleEvent(event);
}

void BoxController::viewWillAppear() {
  if (header()->selectedButton() >= 0) {
    header()->setSelectedButton(-1);
  }
  MultipleDataViewController::viewWillAppear();
}

void BoxController::didEnterResponderChain(Responder * firstResponder) {
  assert(selectedSeriesIndex() >= 0);
  if (!multipleDataView()->dataViewAtIndex(selectedSeriesIndex())->isMainViewSelected()) {
    header()->setSelectedButton(0);
  }
}

void BoxController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    if (header()->selectedButton() >= 0) {
      header()->setSelectedButton(-1);
      return;
    }
    assert(selectedSeriesIndex() >= 0);
  }
  MultipleDataViewController::willExitResponderChain(nextFirstResponder);
}

bool BoxController::moveSelectionHorizontally(int deltaIndex) {
  int selectedBoxCalculation = (int)m_view.dataViewAtIndex(selectedSeriesIndex())->selectedBoxCalculation();
  int nextSelectedCalculation = selectedBoxCalculation + deltaIndex;
  if (m_view.dataViewAtIndex(selectedSeriesIndex())->selectCalculation(nextSelectedCalculation)) {
    reloadBannerView();
    return true;
  }
  return false;
}

void BoxController::reloadBannerView() {
  int series = selectedSeriesIndex();
  if (series < 0) {
    return;
  }

  int selectedBoxCalculation = (int)m_view.dataViewAtIndex(series)->selectedBoxCalculation();

  // Set series name
  char seriesChar = '0' + series + 1;
  char bufferName[] = {' ', 'V', seriesChar, '/', 'N', seriesChar, 0};
  m_view.bannerView()->seriesName()->setText(bufferName);

  // Set calculation name
  m_view.bannerView()->calculationName()->setMessage(m_store->boxPlotCalculationMessageAtIndex(series, selectedBoxCalculation));

  // Set calculation result
  assert(UTF8Decoder::CharSizeOfCodePoint(' ') == 1);
  int precision = Preferences::sharedPreferences()->numberOfSignificantDigits();
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(Poincare::PrintFloat::k_numberOfStoredSignificantDigits) + 1;
  char buffer[bufferSize];
  double calculation = m_store->boxPlotCalculationAtIndex(series, selectedBoxCalculation);
  int numberOfChar = PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize, precision);
  buffer[numberOfChar++] = ' ';
  assert(numberOfChar <= bufferSize - 1);
  buffer[numberOfChar] = 0;
  m_view.bannerView()->calculationValue()->setText(buffer);

  m_view.bannerView()->reload();
}

}
