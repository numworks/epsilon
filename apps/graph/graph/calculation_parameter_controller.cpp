#include "calculation_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Graph {

CalculationParameterController::CalculationParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, BannerView * bannerView, InteractiveCurveViewRange * range, CurveViewCursor * cursor) :
  ViewController(parentResponder),
  m_preimageCell(I18n::Message::Preimage),
  m_selectableTableView(this),
  m_record(),
  m_preimageParameterController(nullptr, inputEventHandlerDelegate, range, cursor, &m_preimageGraphController),
  m_preimageGraphController(nullptr, graphView, bannerView, range, cursor),
  m_tangentGraphController(nullptr, graphView, bannerView, range, cursor),
  m_integralGraphController(nullptr, inputEventHandlerDelegate, graphView, range, cursor),
  m_minimumGraphController(nullptr, graphView, bannerView, range, cursor),
  m_maximumGraphController(nullptr, graphView, bannerView, range, cursor),
  m_rootGraphController(nullptr, graphView, bannerView, range, cursor),
  m_intersectionGraphController(nullptr, graphView, bannerView, range, cursor)
{
}

const char * CalculationParameterController::title() {
  return I18n::translate(I18n::Message::Compute);
}

View * CalculationParameterController::view() {
  return &m_selectableTableView;
}

void CalculationParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

void CalculationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool CalculationParameterController::handleEvent(Ion::Events::Event event) {
  int row = selectedRow();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && row == 0)) {
    static ViewController * controllers[] = {&m_preimageParameterController, &m_intersectionGraphController, &m_maximumGraphController, &m_minimumGraphController, &m_rootGraphController, &m_tangentGraphController, &m_integralGraphController};
    int displayIntersection = shouldDisplayIntersection();
    int indexController = row == 0 ? 0 : row + !displayIntersection;
    ViewController * controller = controllers[indexController];
    if (row == 0) {
      m_preimageParameterController.setRecord(m_record);
    } else if (row == 4 + displayIntersection) {
      m_tangentGraphController.setRecord(m_record);
    } else if (row == 5 + displayIntersection) {
      m_integralGraphController.setRecord(m_record);
    } else {
      static_cast<CalculationGraphController *>(controller)->setRecord(m_record);
    }
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    if (row > 0) {
      stack->pop();
      stack->pop();
    }
    stack->push(controller);
    return true;
  }
  if (event == Ion::Events::Left) {
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}

int CalculationParameterController::numberOfRows() const {
  // Inverse row + [optional intersection row] + all other rows (max, min zeros, tangent, integral)
  return 1 + shouldDisplayIntersection() + k_totalNumberOfReusableCells - 1;
};

KDCoordinate CalculationParameterController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

HighlightCell * CalculationParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < reusableCellCount(type));
  if (type == 0) {
    return &m_cells[index];
  }
  assert(type == 1);
  return &m_preimageCell;
}

int CalculationParameterController::reusableCellCount(int type) {
  if (type == 0) {
    return k_totalNumberOfReusableCells;
  }
  return 1;
}

int CalculationParameterController::typeAtLocation(int i, int j) {
  assert(i == 0);
  return j == 0;
}

void CalculationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index <= numberOfRows());
  if (cell != &m_preimageCell) {
    I18n::Message titles[] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Tangent, I18n::Message::Integral};
    static_cast<MessageTableCell<> *>(cell)->setMessage(titles[index - 1 + !shouldDisplayIntersection()]);
  }
}

void CalculationParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

bool CalculationParameterController::shouldDisplayIntersection() const {
  ContinuousFunctionStore * store = App::app()->functionStore();
  int numberOfCartesianFunctions = store->numberOfActiveFunctionsOfType(Shared::ContinuousFunction::PlotType::Cartesian);
  // Intersection row is displayed when all functions are cartesian and there are least two of them
  // TODO: compute intersections between polar/parametric/cartesian functions?
  return numberOfCartesianFunctions > 1 && numberOfCartesianFunctions == store->numberOfActiveFunctions();
}

}
