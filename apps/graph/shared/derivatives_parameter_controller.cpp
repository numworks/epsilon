#include "derivatives_parameter_controller.h"

#include "../app.h"

using namespace Escher;

namespace Graph {

DerivativesParameterController::DerivativesParameterController(
    Responder *parentResponder)
    : UniformSelectableListController(parentResponder) {
  constexpr I18n::Message messages[k_numberOfCells] = {
      I18n::Message::FirstDerivativeValue,
      I18n::Message::FirstDerivativePlot,
      I18n::Message::SecondDerivativeValue,
      I18n::Message::SecondDerivativePlot,
  };
  for (int row = 0; row < k_numberOfCells; row++) {
    cell(row)->label()->setMessage(messages[row]);
  }
}

bool DerivativesParameterController::handleEvent(Ion::Events::Event event) {
  // Cells are all the same so then are activated by the same events
  if (cell(0)->canBeActivatedByEvent(event)) {
    invertSwitchState(selectedRow());
    m_selectableListView.reloadSelectedCell();
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

void DerivativesParameterController::viewWillAppear() {
  assert(!m_record.isNull());
  for (int row = 0; row < k_numberOfCells; row++) {
    updateSwitch(row);
  }
  UniformSelectableListController::viewWillAppear();
}

StackViewController *DerivativesParameterController::stackController() const {
  return static_cast<StackViewController *>(parentResponder());
}

Shared::ExpiringPointer<Shared::ContinuousFunction>
DerivativesParameterController::function() const {
  assert(!m_record.isNull());
  return App::app()->functionStore()->modelForRecord(m_record);
}

void DerivativesParameterController::updateSwitch(int row) {
  assert(0 <= row && row < k_numberOfCells);
  cell(row)->accessory()->setState(switchState(row));
}

void DerivativesParameterController::invertSwitchState(int row) {
  assert(0 <= row && row < k_numberOfCells);
  switch (row) {
    case k_indexOfFirstDerivativeValue:
      function()->setDisplayDerivative(!function()->displayDerivative());
      break;
    case k_indexOfFirstDerivativePlot:
      break;
    case k_indexOfSecondDerivativeValue:
      break;
    default:
      assert(row == k_indexOfSecondDerivativePlot);
      break;
  }
  updateSwitch(row);
}

bool DerivativesParameterController::switchState(int row) const {
  assert(0 <= row && row < k_numberOfCells);
  switch (row) {
    case k_indexOfFirstDerivativeValue:
      return function()->displayDerivative();
    case k_indexOfFirstDerivativePlot:
      return false;
    case k_indexOfSecondDerivativeValue:
      return false;
    default:
      assert(row == k_indexOfSecondDerivativePlot);
      return false;
  }
}

}  // namespace Graph
