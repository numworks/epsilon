#include "derivative_column_parameter_controller.h"

#include "../app.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

DerivativeColumnParameterController::DerivativeColumnParameterController(
    Responder* parentResponder)
    : ColumnParameterController(parentResponder),
      m_colorParameterController(nullptr),
      m_parameterDelegate(nullptr) {
  m_colorCell.label()->setMessage(I18n::Message::Color);
}

const char* DerivativeColumnParameterController::title() const {
  assert(m_parameterDelegate);
  return m_parameterDelegate->type() == ParameterDelegate::Values
             ? m_titleBuffer
             : I18n::translate(I18n::Message::Options);
}

void DerivativeColumnParameterController::viewWillAppear() {
  assert(m_parameterDelegate);
  m_hideCell.label()->setMessage(m_parameterDelegate->type() ==
                                         ParameterDelegate::DerivativeType::Plot
                                     ? I18n::Message::DeleteDerivative
                                     : I18n::Message::HideDerivativeValues);
  m_colorCell.setVisible(m_parameterDelegate->type() ==
                         ParameterDelegate::DerivativeType::Plot);
  m_colorCell.subLabel()->setMessage(
      ColorNames::NameForColor(function()->color()));
  ColumnParameterController::viewWillAppear();
}

bool DerivativeColumnParameterController::handleEvent(
    Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (cell == &m_colorCell && m_colorCell.canBeActivatedByEvent(event)) {
    m_colorParameterController.setRecord(m_record);
    stack->push(&m_colorParameterController);
    return true;
  }
  if (cell == &m_hideCell && m_hideCell.canBeActivatedByEvent(event)) {
    assert(m_parameterDelegate);
    m_parameterDelegate->hideDerivative(m_record);
    return true;
  }
  return false;
}

void ParameterDelegate::hideDerivative(Ion::Storage::Record record) {
  OMG::ExpiringPointer<ContinuousFunction> f =
      Shared::GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(
          record);
  assert(f->isDerivative());
  int derivationOrder = f->derivationOrder();
  if (type() == DerivativeType::Plot) {
    f->parentFunction()->setDisplayPlotDerivative(derivationOrder, false);
  } else {
    assert(type() == DerivativeType::Values);
    f->parentFunction()->setDisplayValueDerivative(derivationOrder, false);
  }
}

const HighlightCell* DerivativeColumnParameterController::cell(int row) const {
  assert(0 <= row && row <= numberOfRows());
  if (row == 0) {
    return &m_colorCell;
  }
  return &m_hideCell;
}

void DerivativeColumnParameterController::setRecord(
    Ion::Storage::Record record) {
  m_record = record;
  selectRow(0);
}

OMG::ExpiringPointer<ContinuousFunction>
DerivativeColumnParameterController::function() const {
  return Shared::GlobalContextAccessor::ContinuousFunctionStore()
      .modelForRecord(m_record);
}

Shared::ClearColumnHelper*
DerivativeColumnParameterController::clearColumnHelper() {
  return App::app()->valuesController();
}

}  // namespace Graph
