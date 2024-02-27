#include "derivative_column_parameter_controller.h"

#include "../app.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

DerivativeColumnParameterController::DerivativeColumnParameterController(
    Responder* parentResponder)
    : ColumnParameterController(parentResponder),
      m_colorParameterController(nullptr),
      m_derivationOrder(-1) {
  m_colorCell.label()->setMessage(I18n::Message::Color);
  m_hideColumn.label()->setMessage(I18n::Message::HideDerivativeColumn);
}

void DerivativeColumnParameterController::viewWillAppear() {
  m_colorCell.subLabel()->setMessage(
      ColorNames::NameForColor(function()->color(m_derivationOrder)));
  ColumnParameterController::viewWillAppear();
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
  if (selectedCell() == &m_hideColumn &&
      m_colorCell.canBeActivatedByEvent(event)) {
    valuesController()->selectCellAtLocation(
        valuesController()->selectedColumn() - 1,
        valuesController()->selectedRow());
    if (m_derivationOrder == 1) {
      function()->setDisplayValueFirstDerivative(false);
    } else {
      assert(m_derivationOrder == 2);
      function()->setDisplayValueSecondDerivative(false);
    }
    StackViewController* stack = (StackViewController*)(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}

HighlightCell* DerivativeColumnParameterController::cell(int row) {
  assert(0 <= row && row <= numberOfRows());
  if (row == 0) {
    return &m_colorCell;
  }
  return &m_hideColumn;
}

void DerivativeColumnParameterController::setRecord(Ion::Storage::Record record,
                                                    int derivationOrder) {
  m_record = record;
  m_derivationOrder = derivationOrder;
}

ExpiringPointer<ContinuousFunction>
DerivativeColumnParameterController::function() {
  return Shared::GlobalContext::continuousFunctionStore->modelForRecord(
      m_record);
}

Shared::ValuesController*
DerivativeColumnParameterController::valuesController() {
  return App::app()->valuesController();
}

}  // namespace Graph
