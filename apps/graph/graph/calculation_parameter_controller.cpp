#include "calculation_parameter_controller.h"
#include "graph_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Graph {

CalculationParameterController::CalculationParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, BannerView * bannerView, InteractiveCurveViewRange * range, CurveViewCursor * cursor) :
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_record(),
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

void CalculationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool CalculationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    ViewController * controller = nullptr;
    switch(selectedRow()) {
      case 0:
        m_intersectionGraphController.setRecord(m_record);
        controller = &m_intersectionGraphController;
        break;
      case 1:
        m_maximumGraphController.setRecord(m_record);
        controller = &m_maximumGraphController;
        break;
      case 2:
        m_minimumGraphController.setRecord(m_record);
        controller = &m_minimumGraphController;
        break;
      case 3:
        m_rootGraphController.setRecord(m_record);
        controller = &m_rootGraphController;
        break;
      case 4:
        m_tangentGraphController.setRecord(m_record);
        controller = &m_tangentGraphController;
        break;
      case 5:
        m_integralGraphController.setRecord(m_record);
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

KDCoordinate CalculationParameterController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

HighlightCell * CalculationParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int CalculationParameterController::reusableCellCount(int type) {
  return k_totalNumberOfCells;
}

int CalculationParameterController::typeAtLocation(int i, int j) {
  assert(i == 0);
  return 0;
}

void CalculationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[k_totalNumberOfCells] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Tangent, I18n::Message::Integral};
  myCell->setMessage(titles[index]);
}

void CalculationParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

}
