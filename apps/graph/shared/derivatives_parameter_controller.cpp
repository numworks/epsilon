#include "derivatives_parameter_controller.h"

#include "../app.h"

using namespace Escher;

namespace Graph {

DerivativesParameterController::DerivativesParameterController(
    Responder *parentResponder)
    : UniformSelectableListController(parentResponder) {
  cell(0)->label()->setMessage(I18n::Message::GraphDerivative);
}

bool DerivativesParameterController::handleEvent(Ion::Events::Event event) {
  assert(selectedRow() == 0);
  if (cell(0)->canBeActivatedByEvent(event)) {
    function()->setDisplayDerivative(!function()->displayDerivative());
    updateSwitch();
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
  updateSwitch();
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

void DerivativesParameterController::updateSwitch() {
  cell(0)->accessory()->setState(function()->displayDerivative());
}

}  // namespace Graph
