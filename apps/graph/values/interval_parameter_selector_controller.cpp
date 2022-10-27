#include "interval_parameter_selector_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Escher;

namespace Graph {

IntervalParameterSelectorController::IntervalParameterSelectorController() :
  SelectableListViewController(nullptr)
{
}

const char * IntervalParameterSelectorController::title() {
  return I18n::translate(I18n::Message::IntervalSet);
}

void IntervalParameterSelectorController::viewDidDisappear() {
  /* Deselect the table properly because it needs to be relayouted the next time
   * it appears: the number of rows might change according to the plot type. */
  m_selectableTableView.deselectTable(false);
  m_selectableTableView.setFrame(KDRectZero, false);
}

void IntervalParameterSelectorController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool IntervalParameterSelectorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    StackViewController * stack = (StackViewController *)parentResponder();
    Shared::IntervalParameterController * controller = App::app()->valuesController()->intervalParameterController();
    Shared::FunctionType::SymbolType symbolType = symbolTypeAtRow(selectedRow());
    controller->setTitle(messageForType(symbolType));
    setStartEndMessages(controller, symbolType);
    controller->setInterval(App::app()->intervalForSymbolType(symbolType));
    stack->push(controller);
    return true;
  }
  return false;
}

int IntervalParameterSelectorController::numberOfRows() const {
  int rowCount = 0;
  size_t symbolTypeIndex = 0;
  Shared::FunctionType::SymbolType symbolType;
  while (symbolTypeIndex < k_numberOfSymbolTypes) {
    symbolType = static_cast<Shared::FunctionType::SymbolType>(symbolTypeIndex);
    bool symbolTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsInTableOfSymbolType(symbolType) > 0;
    rowCount += symbolTypeIsShown;
    symbolTypeIndex++;
  }
  return rowCount;
}

HighlightCell * IntervalParameterSelectorController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  return m_intervalParameterCell + index;
}

void IntervalParameterSelectorController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < numberOfRows());
  Shared::FunctionType::SymbolType symbolType = symbolTypeAtRow(index);
  static_cast<MessageTableCellWithChevron *>(cell)->setMessage(messageForType(symbolType));
}

Shared::FunctionType::SymbolType IntervalParameterSelectorController::symbolTypeAtRow(int j) const {
  int rowCount = 0;
  size_t symbolTypeIndex = 0;
  Shared::FunctionType::SymbolType symbolType;
  do {
    symbolType = static_cast<Shared::FunctionType::SymbolType>(symbolTypeIndex);
    bool symbolTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsInTableOfSymbolType(symbolType) > 0;
    if (symbolTypeIsShown && rowCount == j) {
      break;
    }
    rowCount += symbolTypeIsShown;
  } while (symbolTypeIndex++ < k_numberOfSymbolTypes);
  assert(rowCount == j);
  return symbolType;
}

I18n::Message IntervalParameterSelectorController::messageForType(Shared::FunctionType::SymbolType symbolType) {
  switch (symbolType) {
  case Shared::FunctionType::SymbolType::X:
    return I18n::Message::IntervalX;
  case Shared::FunctionType::SymbolType::T:
    return I18n::Message::IntervalT;
  default:
    assert(symbolType == Shared::FunctionType::SymbolType::Theta);
    return I18n::Message::IntervalTheta;
  }
}

void IntervalParameterSelectorController::setStartEndMessages(Shared::IntervalParameterController * controller, Shared::FunctionType::SymbolType symbolType) {
  switch (symbolType) {
  case Shared::FunctionType::SymbolType::X:
    controller->setStartEndMessages(I18n::Message::XStart, I18n::Message::XEnd);
    return;
  case Shared::FunctionType::SymbolType::T:
    controller->setStartEndMessages(I18n::Message::TStart, I18n::Message::TEnd);
    return;
  default:
    assert(symbolType == Shared::FunctionType::SymbolType::Theta);
    controller->setStartEndMessages(I18n::Message::ThetaStart, I18n::Message::ThetaEnd);
  }
}

}
