#include "box_controller.h"
#include "../app.h"
#include <poincare/print.h>

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
  return m_view.moveSelectionHorizontally(selectedSeriesIndex(), deltaIndex);
}

bool BoxController::reloadBannerView() {
  int series = selectedSeriesIndex();
  if (series < 0) {
    return false;
  }

  KDCoordinate previousHeight = m_view.bannerView()->minimalSizeForOptimalDisplay().height();

  // With 7 = KDFont::SmallFont->glyphSize().width()
  constexpr static int k_bufferSize = Ion::Display::Width / 7 - (int)sizeof("V1/N1") + 2;
  char buffer[k_bufferSize] = "";

  // Display series name
  StoreController::FillSeriesName(series, buffer, false);
  m_view.bannerView()->seriesName()->setText(buffer);

  // Display calculation
  int selectedBoxCalculation = m_view.dataViewAtIndex(series)->selectedBoxCalculation();
  double value = m_store->boxPlotCalculationAtIndex(series, selectedBoxCalculation);
  Poincare::Print::customPrintf(
    buffer,
    k_bufferSize,
    "%s%s%*.*ed",
    I18n::translate(m_store->boxPlotCalculationMessageAtIndex(series, selectedBoxCalculation)),
    I18n::translate(I18n::Message::StatisticsColonConvention),
    value, Poincare::Preferences::sharedPreferences()->displayMode(), Poincare::PrintFloat::k_numberOfStoredSignificantDigits
  );
  m_view.bannerView()->calculationValue()->setText(buffer);

  m_view.bannerView()->reload();
  return previousHeight != m_view.bannerView()->minimalSizeForOptimalDisplay().height();
}

}
