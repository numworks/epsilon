#include "derivative_column_parameter_controller.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

DerivativeColumnParameterController::DerivativeColumnParameterController(
    ValuesController* valuesController)
    : CalculusColumnParameterController(I18n::Message::HideDerivativeColumn,
                                        valuesController),
      m_colorParameterController(nullptr),
      m_derivationOrder(-1) {
  m_colorCell.label()->setMessage(I18n::Message::Color);
}

void DerivativeColumnParameterController::viewWillAppear() {
  m_colorCell.subLabel()->setMessage(
      ColorNames::NameForColor(function()->color(m_derivationOrder)));
  CalculusColumnParameterController::viewWillAppear();
}

bool DerivativeColumnParameterController::handleEvent(
    Ion::Events::Event event) {
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (selectedCell() == &m_colorCell &&
      m_colorCell.canBeActivatedByEvent(event)) {
    m_colorParameterController.setRecord(m_record, m_derivationOrder);
    stack->push(&m_colorParameterController);
    return true;
  }
  return CalculusColumnParameterController::handleEvent(event);
}

HighlightCell* DerivativeColumnParameterController::cell(int row) {
  assert(0 <= row && row <= numberOfRows());
  if (row == 0) {
    return &m_colorCell;
  }
  return CalculusColumnParameterController::cell(row);
}

void DerivativeColumnParameterController::hideCalculusColumn() {
  if (m_derivationOrder == 1) {
    function()->setDisplayValueFirstDerivative(false);
    return;
  }
  assert(m_derivationOrder == 2);
  function()->setDisplayValueSecondDerivative(false);
}

ExpiringPointer<ContinuousFunction>
DerivativeColumnParameterController::function() {
  return Shared::GlobalContext::continuousFunctionStore->modelForRecord(
      m_record);
}

}  // namespace Graph
