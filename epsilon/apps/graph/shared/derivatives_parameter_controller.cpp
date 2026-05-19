#include "derivatives_parameter_controller.h"

#include <apps/shared/global_store.h>

#include "../app.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

DerivativesParameterController::DerivativesParameterController(
    Responder* parentResponder)
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
    toggleSwitch(selectedRow());
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
  updateSwitchs();
  selectRow(0);
  m_selectableListView.reloadData();
  UniformSelectableListController::viewWillAppear();
}

void DerivativesParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  if (!m_record.isNull()) {
    bool visible = function()->canPlotDerivatives();
    cell(k_indexOfFirstDerivativePlot)->setVisible(visible);
    cell(k_indexOfSecondDerivativePlot)->setVisible(visible);
  }
}

StackViewController* DerivativesParameterController::stackController() const {
  return static_cast<StackViewController*>(parentResponder());
}

OMG::ExpiringPointer<Shared::ContinuousFunction>
DerivativesParameterController::function() const {
  assert(!m_record.isNull());
  return App::app()->functionStore().modelForRecord(m_record);
}

void DerivativesParameterController::updateSwitchs() {
  for (int row = 0; row < k_numberOfCells; row++) {
    cell(row)->accessory()->setState(switchState(row));
  }
}

void DerivativesParameterController::toggleDerivativePlot(uint8_t order) {
  bool hasPlot = function()->displayPlotDerivative(order);
  if (hasPlot) {
    if (order == 1) {
      function()->setDisplayPlotFirstDerivative(false);
    } else {
      function()->setDisplayPlotSecondDerivative(false);
    }
    // setDisplayPlot(false) coupling also clears value, delete record
    GlobalStore::DeleteDerivativeOfRecord(m_record, order);
  } else {
    // Create record only if neither flag was set
    if (!function()->displayValueDerivative(order)) {
      Ion::Storage::Record::ErrorStatus error =
          App::app()->functionStore().addDerivativeModel(m_record, order);
      if (error != Ion::Storage::Record::ErrorStatus::None) {
        return;
      }
    }
    if (order == 1) {
      function()->setDisplayPlotFirstDerivative(true);
    } else {
      function()->setDisplayPlotSecondDerivative(true);
    }
  }
}

void DerivativesParameterController::toggleDerivativeValue(uint8_t order) {
  bool hasValue = function()->displayValueDerivative(order);
  if (hasValue) {
    if (order == 1) {
      function()->setDisplayValueFirstDerivative(false);
    } else {
      function()->setDisplayValueSecondDerivative(false);
    }
    // If plot is also off, delete record
    if (!function()->displayPlotDerivative(order)) {
      GlobalStore::DeleteDerivativeOfRecord(m_record, order);
    }
  } else {
    // Create record only if plot is also off
    if (!function()->displayPlotDerivative(order)) {
      Ion::Storage::Record::ErrorStatus error =
          App::app()->functionStore().addDerivativeModel(m_record, order);
      if (error != Ion::Storage::Record::ErrorStatus::None) {
        return;
      }
    }
    if (order == 1) {
      function()->setDisplayValueFirstDerivative(true);
    } else {
      function()->setDisplayValueSecondDerivative(true);
    }
  }
}

void DerivativesParameterController::toggleSwitch(int row) {
  assert(0 <= row && row < k_numberOfCells);
  switch (row) {
    case k_indexOfFirstDerivativeValue:
      toggleDerivativeValue(1);
      break;
    case k_indexOfFirstDerivativePlot:
      toggleDerivativePlot(1);
      break;
    case k_indexOfSecondDerivativeValue:
      toggleDerivativeValue(2);
      break;
    default:
      assert(row == k_indexOfSecondDerivativePlot);
      toggleDerivativePlot(2);
      break;
  }
  updateSwitchs();
}

bool DerivativesParameterController::switchState(int row) const {
  assert(0 <= row && row < k_numberOfCells);
  switch (row) {
    case k_indexOfFirstDerivativeValue:
      return function()->displayValueFirstDerivative();
    case k_indexOfFirstDerivativePlot:
      return function()->displayPlotFirstDerivative();
    case k_indexOfSecondDerivativeValue:
      return function()->displayValueSecondDerivative();
    default:
      assert(row == k_indexOfSecondDerivativePlot);
      return function()->displayPlotSecondDerivative();
  }
}

}  // namespace Graph
