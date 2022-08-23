#include "calculation_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
#include <assert.h>
#include <cmath>

using namespace Shared;
using namespace Escher;

namespace Graph {

CalculationParameterController::CalculationParameterController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, BannerView * bannerView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, GraphController * graphController) :
  SelectableListViewController(parentResponder),
  m_preimageCell(I18n::Message::Preimage),
  m_graphController(graphController),
  m_preimageParameterController(nullptr, inputEventHandlerDelegate, range, cursor, &m_preimageGraphController),
  m_preimageGraphController(nullptr, graphView, bannerView, range, cursor),
  m_derivativeCell(I18n::Message::GraphDerivative),
  m_tangentGraphController(nullptr, graphView, bannerView, range, cursor),
  m_integralGraphController(nullptr, inputEventHandlerDelegate, graphView, range, cursor),
  m_areaGraphController(nullptr, inputEventHandlerDelegate, graphView, range, cursor),
  m_minimumGraphController(nullptr, graphView, bannerView, range, cursor),
  m_maximumGraphController(nullptr, graphView, bannerView, range, cursor),
  m_rootGraphController(nullptr, graphView, bannerView, range, cursor),
  m_intersectionGraphController(nullptr, graphView, bannerView, range, cursor)
{
}

const char * CalculationParameterController::title() {
  return I18n::translate(I18n::Message::Compute);
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
    static ViewController * controllers[] = {&m_preimageParameterController, &m_intersectionGraphController, &m_maximumGraphController, &m_minimumGraphController, &m_rootGraphController, &m_tangentGraphController, &m_integralGraphController, &m_areaGraphController};
    int displayIntersection = shouldDisplayIntersection();
    if (row == k_derivativeRowIndex + displayIntersection) {
      m_graphController->setDisplayDerivativeInBanner(!m_graphController->displayDerivativeInBanner());
      m_selectableTableView.reloadData();
      return true;
    } else if (row > k_derivativeRowIndex + displayIntersection) {
      row--;
    }
    int indexController = row == 0 ? 0 : row + !displayIntersection;
    ViewController * controller = controllers[indexController];
    if (row == 0) {
      m_preimageParameterController.setRecord(m_record);
    } else if (row == k_derivativeRowIndex + displayIntersection) {
      m_tangentGraphController.setRecord(m_record);
    } else if (row == k_derivativeRowIndex + displayIntersection + 1) {
      m_integralGraphController.setRecord(m_record);
    } else if (row == k_derivativeRowIndex + displayIntersection + 2) {
      m_areaGraphController.setRecord(m_record);
    } else {
      static_cast<CalculationGraphController *>(controller)->setRecord(m_record);
    }
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    if (row > 0) {
      /* setupActiveViewController() must be called here because the graph view
       * must be re-layouted before pushing the controller */
      stack->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, true);
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
  // Inverse row + [optional intersection row] + derivative + all other rows (max, min zeros, derivative, tangent, integral)
  return 1 + shouldDisplayIntersection() + 1 + k_totalNumberOfReusableCells - 1;
};

HighlightCell * CalculationParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < reusableCellCount(type));
  if (type == k_defaultCellType) {
    return &m_cells[index];
  }
  if (type == k_derivativeCellType) {
    return &m_derivativeCell;
  }
  assert(type == k_preImageCellType);
  return &m_preimageCell;
}

int CalculationParameterController::reusableCellCount(int type) {
  if (type == k_defaultCellType) {
    return k_totalNumberOfReusableCells;
  }
  return 1;
}

void CalculationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index <= numberOfRows());
  if (cell == &m_derivativeCell) {
    m_derivativeCell.setState(m_graphController->displayDerivativeInBanner());
  } else if (cell != &m_preimageCell) {
    I18n::Message titles[] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Default, I18n::Message::Tangent, I18n::Message::Integral, I18n::Message::Integral};
    static_cast<MessageTableCell *>(cell)->setMessage(titles[index - 1 + !shouldDisplayIntersection()]);
  }
}

void CalculationParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

bool CalculationParameterController::shouldDisplayIntersection() const {
  /* Intersection is handled between all active functions having one subcurve,
   * except Polar and Parametric. */
  ContinuousFunctionStore * store = App::app()->functionStore();
  /* Intersection row is displayed if there is at least two intersectable
   * functions. */
  return store->numberOfIntersectableFunctions() > 1;
}

}
