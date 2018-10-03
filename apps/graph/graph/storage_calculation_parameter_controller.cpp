#include "storage_calculation_parameter_controller.h"
#include "storage_graph_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Graph {

StorageCalculationParameterController::StorageCalculationParameterController(Responder * parentResponder, StorageGraphView * graphView, BannerView * bannerView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, StorageCartesianFunctionStore * functionStore) :
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_function(nullptr),
  m_tangentGraphController(nullptr, graphView, bannerView, range, cursor),
  m_integralGraphController(nullptr, nullptr /*TODO graphView*/, range, cursor),
  m_minimumGraphController(nullptr, nullptr /*TODO graphView*/, bannerView, range, cursor),
  m_maximumGraphController(nullptr, nullptr /*TODO graphView*/, bannerView, range, cursor),
  m_rootGraphController(nullptr, nullptr /*TODO graphView*/, bannerView, range, cursor),
  m_intersectionGraphController(nullptr, nullptr /*TODO <D-k>graphView*/, bannerView, range, cursor, nullptr /*TODO functionstore*/)
{
}

const char * StorageCalculationParameterController::title() {
  return I18n::translate(I18n::Message::Compute);
}

View * StorageCalculationParameterController::view() {
  return &m_selectableTableView;
}

void StorageCalculationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool StorageCalculationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    ViewController * controller = nullptr;
    switch(selectedRow()) {
      case 0:
        //TODO m_intersectionGraphController.setFunction(m_function);
        controller = &m_intersectionGraphController;
        break;
      case 1:
        //TODO m_maximumGraphController.setFunction(m_function);
        controller = &m_maximumGraphController;
        break;
      case 2:
        //TODO m_minimumGraphController.setFunction(m_function);
        controller = &m_minimumGraphController;
        break;
      case 3:
        //TODO m_rootGraphController.setFunction(m_function);
        controller = &m_rootGraphController;
        break;
      case 4:
        m_tangentGraphController.setFunction(m_function);
        controller = &m_tangentGraphController;
        break;
      case 5:
        //TODO m_integralGraphController.setFunction(m_function);
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

int StorageCalculationParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};


HighlightCell * StorageCalculationParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int StorageCalculationParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate StorageCalculationParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void StorageCalculationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[k_totalNumberOfCells] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Tangent, I18n::Message::Integral};
  myCell->setMessage(titles[index]);
}

void StorageCalculationParameterController::setFunction(StorageCartesianFunction * function) {
  m_function = function;
}

}
