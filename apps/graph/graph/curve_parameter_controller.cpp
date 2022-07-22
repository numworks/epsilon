#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

CurveParameterController::CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController) :
  FloatParameterControllerWithoutButton<float,BufferTableCellWithEditableText>(parentResponder()),
  m_graphController(graphController),
  m_graphRange(graphRange),
  m_cursor(cursor),
  m_preimageGraphController(nullptr, graphView, bannerView, graphRange, cursor),
  m_parameterCells {{&m_selectableTableView, inputEventHandlerDelegate, this}, {&m_selectableTableView, inputEventHandlerDelegate, this}, {&m_selectableTableView, inputEventHandlerDelegate, this}},
  m_calculationCell(I18n::Message::Compute),
  m_calculationParameterController(this, inputEventHandlerDelegate, graphView, bannerView, graphRange, cursor, graphController)
{
}

Shared::ExpiringPointer<Shared::ContinuousFunction> CurveParameterController::function() const {
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
  I18n::Message name = I18n::Message::Default;
  if (index < function()->numberOfCurveParameters()) {
    ContinuousFunction::CurveParameter parameter = function()->getCurveParameter(index);
    name = parameter.parameterName;
    m_parameterCells[index].setEditable(parameter.editable);
  }
  if (name != I18n::Message::Default) {
    m_parameterCells[index].setMessageWithPlaceholder(name);
    FloatParameterControllerWithoutButton::willDisplayCellForIndex(cell, index);
    return;
  }
  if (isDerivative(index)) {
    m_parameterCells[index].setEditable(false);
  }
  if (index == k_preimageIndex || (isDerivative(index))) {
    // The parameter requires a custom name built from the function name
    assert(function()->plotType() == ContinuousFunction::PlotType::Cartesian);
    constexpr size_t bufferSize = BufferTextView::k_maxNumberOfChar;
    char buffer[bufferSize];
    if (index == k_preimageIndex) {
      function()->nameWithArgument(buffer, bufferSize);
    } else {
      assert(index == k_derivativeIndex);
      function()->derivativeNameWithArgument(buffer, bufferSize);
    }
    m_parameterCells[index].setLabelText(buffer);
    FloatParameterControllerWithoutButton::willDisplayCellForIndex(cell, index);
  }
}

float CurveParameterController::parameterAtIndex(int index) {
  if (isDerivative(index)) {
    assert(function()->plotType() == ContinuousFunction::PlotType::Cartesian);
    return function()->approximateDerivative(m_cursor->x(), App::app()->localContext());
  }
  return function()->evaluateCurveParameter(index, m_cursor->t(), m_cursor->x(), m_cursor->y(), App::app()->localContext());
}

bool CurveParameterController::confirmParameterAtIndex(int parameterIndex, double f) {
  if (function()->getCurveParameter(parameterIndex).isPreimage) {
    m_preimageGraphController.setImage(f);
    return true;
  }
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth = (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::getValueDisplayedOnBanner(f, App::app()->localContext(), Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits(), pixelWidth, false);

  Poincare::Coordinate2D<double> xy = function()->evaluateXYAtParameter(f, App::app()->localContext());
  m_cursor->moveTo(f, xy.x1(), xy.x2());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  /* The range might have evolved to center around the cursor but we don't want
   * to reinit the cursor position when displaying the graph controller. To
   * prevent this, we update the snapshot range version in order to make the
   * graph controller as if the range has not evolved since last appearance. */
  uint32_t * snapshotRangeVersion = App::app()->snapshot()->rangeVersion();
  *snapshotRangeVersion = m_graphRange->rangeChecksum();
  return true;
}

bool CurveParameterController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  int index = selectedRow();
  if (!FloatParameterControllerWithoutButton::textFieldDidFinishEditing(textField, text, event)) {
    return false;
  }
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->popUntilDepth(InteractiveCurveViewController::k_graphControllerStackDepth, true);
  if (function()->getCurveParameter(index).isPreimage) {
    stack->push(&m_preimageGraphController);
  }
  return true;
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (shouldDisplayCalculation() && selectedRow() == numberOfRows() - 1) {
      m_calculationParameterController.setRecord(m_record);
      stack->push(&m_calculationParameterController);
      return true;
    }
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

bool CurveParameterController::editableParameter(int index) {
  return !isDerivative(index) && function()->getCurveParameter(index).editable;
}

int CurveParameterController::numberOfRows() const {
  return numberOfParameters() + shouldDisplayCalculation();
}

void CurveParameterController::viewWillAppear() {
  m_preimageGraphController.setImage(m_cursor->y());
  resetMemoization();
  m_selectableTableView.reloadData();
  SelectableListViewController::viewWillAppear();
}

bool CurveParameterController::shouldDisplayCalculation() const {
  return function()->canDisplayDerivative();
}

bool CurveParameterController::shouldDisplayDerivative() const {
  return function()->canDisplayDerivative() && m_graphController->displayDerivativeInBanner();
}

bool CurveParameterController::isDerivative(int index) const {
  return shouldDisplayDerivative() && index == k_derivativeIndex;
}

}
