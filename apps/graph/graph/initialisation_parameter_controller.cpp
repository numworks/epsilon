#include "initialisation_parameter_controller.h"
#include "../../apps_container.h"
#include <assert.h>
#include <math.h>

using namespace Shared;

namespace Graph {

InitialisationParameterController::InitialisationParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_graphRange(graphRange)
{
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
    if (m_selectableTableView.selectedRow() == 0) {
      AppsContainer * container = (AppsContainer *)app()->container();
      m_graphRange->setTrigonometric(container->preferences()->angleUnit());
    } else {
      RangeMethodPointer rangeMethods[k_totalNumberOfCells-1] = {&InteractiveCurveViewRange::roundAbscissa,
        &InteractiveCurveViewRange::normalize, &InteractiveCurveViewRange::setDefault};
      (m_graphRange->*rangeMethods[m_selectableTableView.selectedRow()-1])();
    }
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
  return Metric::ParameterCellHeight;
}

void InitialisationParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  const char * titles[4] = {"Trigonometrique", "Abscisses entieres", "Orthonorme", "Reglage de base"};
  myCell->setText(titles[index]);
}

}
