#include "calculation_parameter_controller.h"
#include "graph_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Graph {

CalculationParameterController::CalculationParameterController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, CartesianFunctionStore * functionStore) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_function(nullptr),
  m_tangentGraphController(nullptr, graphView, bannerView, range, cursor),
  m_integralGraphController(nullptr, graphView, range, cursor),
  m_minimumGraphController(nullptr, graphView, bannerView, range, cursor),
  m_maximumGraphController(nullptr, graphView, bannerView, range, cursor),
  m_rootGraphController(nullptr, graphView, bannerView, range, cursor),
  m_intersectionGraphController(nullptr, graphView, bannerView, range, cursor, functionStore)
{
}

const char * CalculationParameterController::title() {
  return I18n::translate(I18n::Message::Compute);
}

View * CalculationParameterController::view() {
  return &m_selectableTableView;
}

void CalculationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool CalculationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    ViewController * controller = nullptr;
    switch(selectedRow()) {
      case 0:
        m_intersectionGraphController.setFunction(m_function);
        controller = &m_intersectionGraphController;
        break;
      case 1:
        m_maximumGraphController.setFunction(m_function);
        controller = &m_maximumGraphController;
        break;
      case 2:
        m_minimumGraphController.setFunction(m_function);
        controller = &m_minimumGraphController;
        break;
      case 3:
        m_rootGraphController.setFunction(m_function);
        controller = &m_rootGraphController;
        break;
      case 4:
        m_tangentGraphController.setFunction(m_function);
        controller = &m_tangentGraphController;
        break;
      case 5:
        m_integralGraphController.setFunction(m_function);
        controller = &m_integralGraphController;
        break;
      default:
        return false;
    }
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    stack->pop();
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

int CalculationParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};


HighlightCell * CalculationParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int CalculationParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate CalculationParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void CalculationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[k_totalNumberOfCells] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Tangent, I18n::Message::Integral};
  myCell->setMessage(titles[index]);
}

void CalculationParameterController::setFunction(CartesianFunction * function) {
  m_function = function;
}

}
