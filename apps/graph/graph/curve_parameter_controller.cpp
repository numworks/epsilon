#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

CurveParameterController::CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController) :
  FloatParameterControllerWithoutButton<float>(parentResponder()),
  m_graphRange(graphRange),
  m_cursor(cursor),
  m_preimageGraphController(nullptr, graphView, bannerView, graphRange, cursor),
  m_parameterCells {{&m_selectableTableView, inputEventHandlerDelegate, this}, {&m_selectableTableView, inputEventHandlerDelegate, this}, {&m_selectableTableView, inputEventHandlerDelegate, this}},
  m_calculationCell(I18n::Message::Compute),
  m_calculationParameterController(this, inputEventHandlerDelegate, graphView, bannerView, graphRange, cursor, graphController)
{
}

Shared::ExpiringPointer<Shared::ContinuousFunction> CurveParameterController::function() {
  return App::app()->functionStore()->modelForRecord(m_record);
}

const char * CurveParameterController::title() {
  return I18n::translate(I18n::Message::PlotOptions);
}

void CurveParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void CurveParameterController::willDisplayCellForIndex(HighlightCell *cell, int index) {
  if (index == 0) {
    m_parameterCells[index].setMessage(function()->parameterMessageName());
    FloatParameterControllerWithoutButton::willDisplayCellForIndex(cell, index);
  } else if (index == 1) {
    m_parameterCells[index].setMessage(I18n::Message::Y);
    FloatParameterControllerWithoutButton::willDisplayCellForIndex(cell, index);
  }
}

float CurveParameterController::parameterAtIndex(int index) {
  if (index==0) {
    return m_cursor->t();
  }
  assert(index==1);
  return m_preimageGraphController.image();
}

bool CurveParameterController::confirmParameterAtIndex(int parameterIndex, double f) {
  if (parameterIndex == 0) {
  FunctionApp * myApp = FunctionApp::app();
  ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth = (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::getValueDisplayedOnBanner(f, myApp->localContext(), Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits(), pixelWidth, false);

  Poincare::Coordinate2D<double> xy = function->evaluateXYAtParameter(f, myApp->localContext());
  m_cursor->moveTo(f, xy.x1(), xy.x2());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  /* The range might have evolved to center around the cursor but we don't want
   * to reinit the cursor position when displaying the graph controller. To
   * prevent this, we update the snapshot range version in order to make the
   * graph controller as if the range has not evolved since last appearance. */
  uint32_t * snapshotRangeVersion = static_cast<FunctionApp::Snapshot *>(myApp->snapshot())->rangeVersion();
  *snapshotRangeVersion = m_graphRange->rangeChecksum();
  return true;
  }
  assert(parameterIndex == 1);
  m_preimageGraphController.setImage(f);
  return true;
}

bool CurveParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  int row = selectedRow();
  if (!FloatParameterControllerWithoutButton::textFieldDidFinishEditing(textField, text, event)) {
    return false;
  }
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (row == 0 || row == 1) {
    stack->popUntilDepth(InteractiveCurveViewController::k_graphControllerStackDepth, true);
  }
  if (row == 1) {
    //m_preimageGraphController.setRecord(m_record);
    stack->push(&m_preimageGraphController);
  }
  return true;
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  int index;
  if (shouldDisplayCalculation()) {
    index = selectedRow();
  } else {
    assert(selectedRow() == 0);
    index = 1;
  }
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && (index == 0 || index == 1))) {
    switch (index) {
      case 0:
      {
//        stack->popUntilDepth(InteractiveCurveViewController::k_graphControllerStackDepth, true);
        return false;
      }
      case 1: // TODO -1
      {
        m_calculationParameterController.setRecord(m_record);
        stack->push(&m_calculationParameterController);
        return true;
      }
      default:
        assert(false);
        return false;
    }
  }
  if (event == Ion::Events::Left
   && stack->depth() > Shared::InteractiveCurveViewController::k_graphControllerStackDepth + 1)
  {
    /* We only allow popping with Left if there is another menu beneath this
     * one. */
    stack->pop();
    return true;
  }
  return false;
}

Escher::HighlightCell * CurveParameterController::reusableCell(int index, int type) {
  if (type == k_calculationCellType) {
    return &m_calculationCell;
  }
  return &m_parameterCells[index];
}

int CurveParameterController::typeAtIndex(int index) {
  if (index < numberOfParameters()) {
    return k_parameterCellType;
  }
  return k_calculationCellType;
}

int CurveParameterController::numberOfRows() const {
  return numberOfParameters() + shouldDisplayCalculation();
};

void CurveParameterController::viewWillAppear() {
  m_preimageGraphController.setImage(m_cursor->y());
  SelectableListViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

bool CurveParameterController::shouldDisplayCalculation() const {
  return App::app()->functionStore()->modelForRecord(m_record)->canDisplayDerivative();
}

}
