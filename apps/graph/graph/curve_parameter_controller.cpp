#include "curve_parameter_controller.h"
#include <assert.h>

namespace Graph {

CurveParameterController::CurveParameterController(GraphView * graphView, GraphWindow * graphWindow) :
  ViewController(nullptr),
  m_graphView(graphView),
  m_function(nullptr),
  m_calculationCell(ChevronMenuListCell((char*)"Calculer")),
  m_goToCell(ChevronMenuListCell((char*)"Aller a")),
  m_derivativeCell(SwitchMenuListCell((char*)"Nombre derivee")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_goToParameterController(GoToParameterController(this, graphWindow))
{
}

const char * CurveParameterController::title() const {
  return "Options de la courbe";
}

View * CurveParameterController::view() {
  return &m_selectableTableView;
}

void CurveParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

void CurveParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (cell == &m_derivativeCell) {
    SwitchView * switchView = (SwitchView *)m_derivativeCell.accessoryView();
    switchView->setState(m_graphView->bannerView()->displayDerivative());
  }
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
        return true;
      case 1:
      {
        m_goToParameterController.setFunction(m_function);
        StackViewController * stack = (StackViewController *)parentResponder();
        stack->push(&m_goToParameterController);
        return true;
      }
      case 2:
      {
        BannerView * bannerView = (BannerView *)m_graphView->bannerView();
        bannerView->setDisplayDerivative(!bannerView->displayDerivative());
        m_selectableTableView.reloadData();
        return true;
      }
      default:
        return false;
    }
  }
  return false;
}

int CurveParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};

TableViewCell * CurveParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  TableViewCell * cells[] = {&m_calculationCell, &m_goToCell, &m_derivativeCell};
  return cells[index];
}

int CurveParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate CurveParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void CurveParameterController::setFunction(Function * function) {
  m_function = function;
}

}
