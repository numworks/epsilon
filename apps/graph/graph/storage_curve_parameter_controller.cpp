#include "storage_curve_parameter_controller.h"
#include "graph_controller.h"
#include "../../i18n.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

StorageCurveParameterController::StorageCurveParameterController(InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, StorageGraphView * graphView, GraphController * graphController, StorageCartesianFunctionStore * functionStore) :
  Shared::StorageFunctionCurveParameterController<StorageCartesianFunction>(graphRange, cursor),
  m_goToParameterController(this, graphRange, cursor, I18n::Message::X),
  m_graphController(graphController),
  m_calculationCell(I18n::Message::Compute),
  m_derivativeCell(I18n::Message::DerivateNumber),
  m_calculationParameterController(this, graphView, bannerView, graphRange, cursor, functionStore)
{
}

const char * StorageCurveParameterController::title() {
  return I18n::translate(I18n::Message::PlotOptions);
}

void StorageCurveParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_derivativeCell) {
    SwitchView * switchView = (SwitchView *)m_derivativeCell.accessoryView();
    switchView->setState(m_graphController->displayDerivativeInBanner());
  }
}

bool StorageCurveParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && (selectedRow() == 0 || selectedRow() == 1))) {
    switch (selectedRow()) {
      case 0:
      {
        m_calculationParameterController.setFunction(&m_function);
        StackViewController * stack = (StackViewController *)parentResponder();
        stack->push(&m_calculationParameterController);
        return true;
      }
      case 1:
        return handleGotoSelection();
      case 2:
      {
        m_graphController->setDisplayDerivativeInBanner(!m_graphController->displayDerivativeInBanner());
        m_selectableTableView.reloadData();
        return true;
      }
      default:
        return false;
    }
  }
  return false;
}

int StorageCurveParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};

HighlightCell * StorageCurveParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  HighlightCell * cells[] = {&m_calculationCell, &m_goToCell, &m_derivativeCell};
  return cells[index];
}

int StorageCurveParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

StorageFunctionGoToParameterController<StorageCartesianFunction> * StorageCurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

}
