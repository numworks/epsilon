#include "interval_parameter_selector_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

namespace Graph {

IntervalParameterSelectorController::IntervalParameterSelectorController() :
  ViewController(nullptr),
  m_selectableTableView(this, this, this)
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
    Shared::ContinuousFunction::PlotType plotType = plotTypeAtRow(selectedRow());
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
  Shared::ContinuousFunction::PlotType plotType;
  while (plotTypeIndex < Shared::ContinuousFunction::k_numberOfPlotTypes) {
    plotType = static_cast<Shared::ContinuousFunction::PlotType>(plotTypeIndex);
    bool plotTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsOfType(plotType) > 0;
    rowCount += plotTypeIsShown;
    plotTypeIndex++;
  }
  return rowCount;
}

HighlightCell * IntervalParameterSelectorController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  return m_intervalParameterCell + index;
}

int IntervalParameterSelectorController::reusableCellCount() const {
  return Shared::ContinuousFunction::k_numberOfPlotTypes;
}

void IntervalParameterSelectorController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < numberOfRows());
  Shared::ContinuousFunction::PlotType plotType = plotTypeAtRow(index);
  static_cast<MessageTableCellWithChevron<> *>(cell)->setMessage(messageForType(plotType));
}

Shared::ContinuousFunction::PlotType IntervalParameterSelectorController::plotTypeAtRow(int j) const {
  int rowCount = 0;
  int plotTypeIndex = 0;
  Shared::ContinuousFunction::PlotType plotType;
  while (plotTypeIndex < Shared::ContinuousFunction::k_numberOfPlotTypes) {
    plotType = static_cast<Shared::ContinuousFunction::PlotType>(plotTypeIndex);
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

I18n::Message IntervalParameterSelectorController::messageForType(Shared::ContinuousFunction::PlotType plotType) {
  constexpr I18n::Message message[Shared::ContinuousFunction::k_numberOfPlotTypes] = {
    I18n::Message::IntervalX,
    I18n::Message::IntervalTheta,
    I18n::Message::IntervalT
  };
  return message[static_cast<size_t>(plotType)];
}

void IntervalParameterSelectorController::setStartEndMessages(Shared::IntervalParameterController * controller, Shared::ContinuousFunction::PlotType plotType) {
  if (plotType == Shared::ContinuousFunction::PlotType::Cartesian) {
    controller->setStartEndMessages(I18n::Message::XStart, I18n::Message::XEnd);
  } else if (plotType == Shared::ContinuousFunction::PlotType::Polar) {
    controller->setStartEndMessages(I18n::Message::ThetaStart, I18n::Message::ThetaEnd);
  } else {
    assert(plotType == Shared::ContinuousFunction::PlotType::Parametric);
    controller->setStartEndMessages(I18n::Message::TStart, I18n::Message::TEnd);
  }
}

}
