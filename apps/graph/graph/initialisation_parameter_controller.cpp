#include "initialisation_parameter_controller.h"
#include <assert.h>
#include <math.h>

namespace Graph {

InitialisationParameterController::InitialisationParameterController(Responder * parentResponder, AxisInterval * axisInterval, GraphView * graphView) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_axisInterval(axisInterval),
  m_graphView(graphView)
{
  m_cells[0].setText("Trigonometrique");
  m_cells[1].setText("Abscisses entieres");
  m_cells[2].setText("Orthonorme");
  m_cells[3].setText("Reglage de base");

}

const char * InitialisationParameterController::title() const {
  return "Zoom predefini";
}

View * InitialisationParameterController::view() {
  return &m_selectableTableView;
}

void InitialisationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool InitialisationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      // TODO: if mode == degree, xmin = -600, xmax = 600
        m_axisInterval->setXMin(-10.5f);
        m_axisInterval->setXMax(10.5f);
        m_axisInterval->setYAuto(false);
        m_axisInterval->setYMin(-1.6f);
        m_axisInterval->setYMax(1.6f);
        break;
      case 1:
      {
        float xMin = m_axisInterval->xMin();
        float xMax =m_axisInterval->xMax();
        m_axisInterval->setXMin(roundf((xMin+xMax)/2) - 160.0f);
        m_axisInterval->setXMax(roundf((xMin+xMax)/2) + 159.0f);
        break;
      }
      case 2:
      {
        float xMin = m_axisInterval->xMin();
        float xMax =m_axisInterval->xMax();
        float yMin = m_axisInterval->yMin();
        float yMax =m_axisInterval->yMax();
        m_axisInterval->setXMin((xMin+xMax)/2 - 5.3f);
        m_axisInterval->setXMax((xMin+xMax)/2 + 5.3f);
        m_axisInterval->setYMin((yMin+yMax)/2 - 3.1f);
        m_axisInterval->setYMax((yMin+yMax)/2 + 3.1f);
        break;
      }
      case 3:
        m_axisInterval->setXMin(-10.0f);
        m_axisInterval->setXMax(10.0f);
        m_axisInterval->setYAuto(true);
        break;
      default:
        return false;
    }
    m_graphView->initCursorPosition();
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->pop();
    return true;
  }
  return false;
}

int InitialisationParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};


TableViewCell * InitialisationParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int InitialisationParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate InitialisationParameterController::cellHeight() {
  return 35;
}

}
