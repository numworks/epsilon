#include "derivative_column_parameter_controller.h"

#include <apps/shared/global_store.h>

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
    m_parameterDelegate->hideDerivative(m_record, m_derivationOrder);
    return true;
  }
  return false;
}

void ParameterDelegate::hideDerivative(Ion::Storage::Record record,
                                       int derivationOrder) {
  if (derivationOrder == -1) {
    /* Standalone derivative record: update the parent's display settings and
     * delete the derivative record */
    OMG::ExpiringPointer<ContinuousFunction> f =
        App::app()->functionStore().modelForRecord(record);
    derivationOrder = f->derivationOrder();
    Ion::Storage::Record parentRecord = f->parentRecord();
    OMG::ExpiringPointer<ContinuousFunction> parent =
        App::app()->functionStore().modelForRecord(parentRecord);
    if (derivationOrder == 1) {
      parent->setDisplayPlotFirstDerivative(false);
    } else {
      assert(derivationOrder == 2);
      parent->setDisplayPlotSecondDerivative(false);
    }
    GlobalStore::DeleteDerivativeOfRecord(parentRecord, derivationOrder);
  } else {
    // Derivative values column: only update the parent's display settings
    OMG::ExpiringPointer<ContinuousFunction> parent =
        App::app()->functionStore().modelForRecord(record);
    if (derivationOrder == 1) {
      parent->setDisplayValueFirstDerivative(false);
    } else {
      assert(derivationOrder == 2);
      parent->setDisplayValueSecondDerivative(false);
    }
  }
}

const HighlightCell* DerivativeColumnParameterController::cell(int row) const {
  assert(0 <= row && row <= numberOfRows());
  if (row == 0) {
    return &m_colorCell;
  }
  return &m_hideCell;
}

void DerivativeColumnParameterController::setRecord(Ion::Storage::Record record,
                                                    int derivationOrder) {
  assert(derivationOrder == -1 || derivationOrder == 1 || derivationOrder == 2);
  m_record = record;
  m_derivationOrder = derivationOrder;
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
