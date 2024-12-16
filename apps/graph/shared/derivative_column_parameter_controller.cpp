#include "derivative_column_parameter_controller.h"

#include "../app.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

DerivativeColumnParameterController::DerivativeColumnParameterController(
    Responder* parentResponder)
    : ColumnParameterController(parentResponder),
      m_colorParameterController(nullptr),
      m_derivationOrder(-1),
      m_parameterDelegate(nullptr) {
  m_colorCell.label()->setMessage(I18n::Message::Color);
}

const char* DerivativeColumnParameterController::title() const {
  assert(m_parameterDelegate);
  return m_parameterDelegate->usePersonalizedTitle()
             ? m_titleBuffer
             : I18n::translate(I18n::Message::Options);
}

void DerivativeColumnParameterController::viewWillAppear() {
  assert(m_parameterDelegate);
  m_hideCell.label()->setMessage(m_parameterDelegate->hideDerivativeMessage());
  m_colorCell.setVisible(m_parameterDelegate->showColorCell());
  m_colorCell.subLabel()->setMessage(
      ColorNames::NameForColor(function()->color(m_derivationOrder)));
  ColumnParameterController::viewWillAppear();
}

bool DerivativeColumnParameterController::handleEvent(
    Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (cell == &m_colorCell && m_colorCell.canBeActivatedByEvent(event)) {
    m_colorParameterController.setRecord(m_record, m_derivationOrder);
    stack->push(&m_colorParameterController);
    return true;
  }
  if (cell == &m_hideCell && m_hideCell.canBeActivatedByEvent(event)) {
    assert(m_parameterDelegate);
    m_parameterDelegate->hideDerivative(m_record, m_derivationOrder);
    return true;
  }
  return false;
}

HighlightCell* DerivativeColumnParameterController::cell(int row) {
  assert(0 <= row && row <= numberOfRows());
  if (row == 0) {
    return &m_colorCell;
  }
  return &m_hideCell;
}

void DerivativeColumnParameterController::setRecord(Ion::Storage::Record record,
                                                    int derivationOrder) {
  m_record = record;
  m_derivationOrder = derivationOrder;
  selectRow(0);
}

ExpiringPointer<ContinuousFunction>
DerivativeColumnParameterController::function() const {
  return Shared::GlobalContext::s_continuousFunctionStore->modelForRecord(
      m_record);
}

Shared::ClearColumnHelper*
DerivativeColumnParameterController::clearColumnHelper() {
  return App::app()->valuesController();
}

}  // namespace Graph
