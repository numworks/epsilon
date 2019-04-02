#include "calculation_parameter_controller.h"
#include "graph_controller.h"
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

void CalculationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool CalculationParameterController::handleEvent(Ion::Events::Event event) {
  int row = selectedRow();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && row == 0)) {
    ViewController * controller = nullptr;
    switch(row) {
      case 0:
        m_preimageParameterController.setRecord(m_record);
        controller = &m_preimageParameterController;
        break;
      case 1:
        m_intersectionGraphController.setRecord(m_record);
        controller = &m_intersectionGraphController;
        break;
      case 2:
        m_maximumGraphController.setRecord(m_record);
        controller = &m_maximumGraphController;
        break;
      case 3:
        m_minimumGraphController.setRecord(m_record);
        controller = &m_minimumGraphController;
        break;
      case 4:
        m_rootGraphController.setRecord(m_record);
        controller = &m_rootGraphController;
        break;
      case 5:
        m_tangentGraphController.setRecord(m_record);
        controller = &m_tangentGraphController;
        break;
      case 6:
        m_integralGraphController.setRecord(m_record);
        controller = &m_integralGraphController;
        break;
      default:
        return false;
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

int CalculationParameterController::numberOfRows() {
  constexpr int k_totalNumberOfCells = k_totalNumberOfReusableCells + 1;
  return k_totalNumberOfCells;
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
  if (cell != &m_preimageCell) {
    I18n::Message titles[] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Tangent, I18n::Message::Integral};
    static_cast<MessageTableCell *>(cell)->setMessage(titles[index - 1]);
  }
}

void CalculationParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

}
