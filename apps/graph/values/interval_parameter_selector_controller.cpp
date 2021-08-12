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
  resetMemoization();
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
    NewFunction::PlotType plotType = plotTypeAtRow(selectedRow());
    controller->setTitle(messageForType(plotType));
    setStartEndMessages(controller, plotType);
    controller->setInterval(App::app()->intervalForType(plotType));
    stack->push(controller);
    return true;
  }
  return false;
}

int IntervalParameterSelectorController::numberOfRows() const {
  int rowCount = 0;
  int plotTypeIndex = 0;
  NewFunction::PlotType plotType;
  while (plotTypeIndex < NewFunction::k_numberOfPlotTypes) {
    plotType = static_cast<NewFunction::PlotType>(plotTypeIndex);
    bool plotTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsOfType(plotType) > 0;
    rowCount += plotTypeIsShown;
    plotTypeIndex++;
  }
  return rowCount;
}

KDCoordinate IntervalParameterSelectorController::nonMemoizedRowHeight(int j) {
  MessageTableCellWithChevron tempCell;
  return heightForCellAtIndex(&tempCell, j);
}

HighlightCell * IntervalParameterSelectorController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  return m_intervalParameterCell + index;
}

int IntervalParameterSelectorController::reusableCellCount(int type) {
  return NewFunction::k_numberOfPlotTypes;
}

void IntervalParameterSelectorController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < numberOfRows());
  NewFunction::PlotType plotType = plotTypeAtRow(index);
  static_cast<MessageTableCellWithChevron *>(cell)->setMessage(messageForType(plotType));
}

NewFunction::PlotType IntervalParameterSelectorController::plotTypeAtRow(int j) const {
  int rowCount = 0;
  int plotTypeIndex = 0;
  NewFunction::PlotType plotType;
  while (plotTypeIndex < NewFunction::k_numberOfPlotTypes) {
    plotType = static_cast<NewFunction::PlotType>(plotTypeIndex);
    bool plotTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsOfType(plotType) > 0;
    if (plotTypeIsShown && rowCount == j) {
      break;
    }
    rowCount += plotTypeIsShown;
    plotTypeIndex++;
  }
  assert(rowCount == j);
  return plotType;
}

I18n::Message IntervalParameterSelectorController::messageForType(NewFunction::PlotType plotType) {
  constexpr I18n::Message message[NewFunction::k_numberOfPlotTypes] = {
    I18n::Message::IntervalX,
    I18n::Message::IntervalTheta,
    I18n::Message::IntervalT
  };
  return message[static_cast<size_t>(plotType)];
}

void IntervalParameterSelectorController::setStartEndMessages(Shared::IntervalParameterController * controller, NewFunction::PlotType plotType) {
  if (plotType == NewFunction::PlotType::Polar) {
    controller->setStartEndMessages(I18n::Message::ThetaStart, I18n::Message::ThetaEnd);
  } else if (plotType == NewFunction::PlotType::Parametric) {
    controller->setStartEndMessages(I18n::Message::TStart, I18n::Message::TEnd);
  } else {
    controller->setStartEndMessages(I18n::Message::XStart, I18n::Message::XEnd);
  }
}

}
