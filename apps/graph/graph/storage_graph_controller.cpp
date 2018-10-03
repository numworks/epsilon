#include "graph_controller.h"
#include "../app.h"

using namespace Shared;

namespace Graph {

StorageGraphController::StorageGraphController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  StorageFunctionGraphController<StorageCartesianFunction>(parentResponder, header, curveViewRange, &m_view, cursor, indexFunctionSelectedByCursor, modelVersion, rangeVersion, angleUnitVersion),
  m_bannerView(),
  m_view(functionStore, curveViewRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(curveViewRange),
  m_curveParameterController(curveViewRange, &m_bannerView, m_cursor, &m_view, this, functionStore),
  m_functionStore(functionStore),
  m_displayDerivativeInBanner(false)
{
  m_graphRange->setDelegate(this);
}

I18n::Message StorageGraphController::emptyMessage() {
  if (m_functionStore->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

void StorageGraphController::viewWillAppear() {
  m_view.drawTangent(false);
  Shared::StorageFunctionGraphController<StorageCartesianFunction>::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor()); // update the color of the cursor
}

bool StorageGraphController::displayDerivativeInBanner() const {
  return m_displayDerivativeInBanner;
}

void StorageGraphController::setDisplayDerivativeInBanner(bool displayDerivative) {
  m_displayDerivativeInBanner = displayDerivative;
}

float StorageGraphController::interestingXRange() {
  float characteristicRange = 0.0f;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    StorageCartesianFunction f = functionStore()->activeFunctionAtIndex(i);
    float fRange = f.reducedExpression(myApp->localContext()).characteristicXRange(*(myApp->localContext()), Poincare::Preferences::sharedPreferences()->angleUnit());
    if (!std::isnan(fRange)) {
      characteristicRange = fRange > characteristicRange ? fRange : characteristicRange;
    }
  }
  return (characteristicRange > 0.0f ? 1.6f*characteristicRange : 10.0f);
}

void StorageGraphController::selectFunctionWithCursor(int functionIndex) {
 Shared::StorageFunctionGraphController<StorageCartesianFunction>::selectFunctionWithCursor(functionIndex);
  StorageCartesianFunction f = m_functionStore->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  m_cursorView.setColor(f.color());
}

BannerView * StorageGraphController::bannerView() {
  return &m_bannerView;
}

void StorageGraphController::reloadBannerView() {
  Shared::StorageFunctionGraphController<StorageCartesianFunction>::reloadBannerView();
  m_bannerView.setNumberOfSubviews(2+m_displayDerivativeInBanner);
  if (m_functionStore->numberOfActiveFunctions() == 0 || !m_displayDerivativeInBanner) {
    return;
  }
  StorageCartesianFunction f = m_functionStore->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  //TODO reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, &f, myApp);
}

bool StorageGraphController::moveCursorHorizontally(int direction) {
  StorageCartesianFunction f = m_functionStore->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, &f, myApp, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

InteractiveCurveViewRange * StorageGraphController::interactiveCurveViewRange() {
  return m_graphRange;
}

StorageCartesianFunctionStore * StorageGraphController::functionStore() const {
  return m_functionStore;
}

StorageGraphView * StorageGraphController::functionGraphView() {
  return &m_view;
}

StorageCurveParameterController * StorageGraphController::curveParameterController() {
  return &m_curveParameterController;
}

}
