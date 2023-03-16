#include "curve_parameter_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/print.h>

#include "../app.h"
#include "apps/shared/color_names.h"
#include "graph_controller.h"

using namespace Shared;
using namespace Escher;

namespace Graph {

CurveParameterController::CurveParameterController(
    Escher::InputEventHandlerDelegate *inputEventHandlerDelegate,
    InteractiveCurveViewRange *graphRange, BannerView *bannerView,
    CurveViewCursor *cursor, GraphView *graphView,
    GraphController *graphController)
    : ExplicitFloatParameterController(parentResponder()),
      m_abscissaCell(&m_selectableListView, inputEventHandlerDelegate, this),
      m_imageCell(&m_selectableListView, inputEventHandlerDelegate, this),
      m_derivativeNumberCell(&m_selectableListView, inputEventHandlerDelegate,
                             this),
      m_calculationCell(I18n::Message::Find),
      m_optionsCell(I18n::Message::Options),
      m_graphController(graphController),
      m_graphRange(graphRange),
      m_cursor(cursor),
      m_preimageGraphController(nullptr, graphView, bannerView, graphRange,
                                cursor),
      m_calculationParameterController(this, inputEventHandlerDelegate,
                                       graphView, bannerView, graphRange,
                                       cursor) {}

Escher::HighlightCell *CurveParameterController::cell(int index) {
  assert(0 <= index && index < k_numberOfRows);
  HighlightCell *cells[k_numberOfRows] = {
      &m_abscissaCell, &m_imageCell,       &m_derivativeNumberCell,
      &m_spacer,       &m_calculationCell, &m_optionsCell};
  return cells[index];
}

Shared::ExpiringPointer<Shared::ContinuousFunction>
CurveParameterController::function() const {
  return App::app()->functionStore()->modelForRecord(m_record);
}

const char *CurveParameterController::title() {
  if (function()->isNamed()) {
    const char *calculate = I18n::translate(I18n::Message::CalculateOnFx);
    size_t len = strlen(calculate);
    memcpy(m_title, calculate, len);
    function()->nameWithArgument(m_title + len, k_titleSize - len);
  } else {
    const char *colorName = I18n::translate(
        Shared::ColorNames::NameForCurveColor(function()->color()));
    Poincare::Print::CustomPrintf(
        m_title, k_titleSize,
        I18n::translate(I18n::Message::CalculateOnTheCurve), colorName);
  }
  return m_title;
}

void CurveParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCell(0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableListView);
}

void CurveParameterController::willDisplayCellForIndex(HighlightCell *cell,
                                                       int index) {
  I18n::Message name = I18n::Message::Default;
  BufferTableCellWithEditableText *parameterCells[] = {
      &m_abscissaCell, &m_imageCell, &m_derivativeNumberCell};
  if (index < function()->properties().numberOfCurveParameters()) {
    ContinuousFunctionProperties::CurveParameter parameter =
        function()->properties().getCurveParameter(index);
    name = parameter.parameterName;
    parameterCells[index]->setEditable(parameter.editable);
  }
  if (name != I18n::Message::Default) {
    parameterCells[index]->setMessageWithPlaceholders(name);
    ExplicitFloatParameterController::willDisplayCellForIndex(cell, index);
    return;
  }
  if (cell == &m_derivativeNumberCell) {
    m_derivativeNumberCell.setEditable(false);
  }
  if (cell == &m_imageCell || cell == &m_derivativeNumberCell) {
    // The parameter requires a custom name built from the function name
    constexpr size_t bufferSize = BufferTextView::k_maxNumberOfChar;
    char buffer[bufferSize];
    if (cell == &m_imageCell) {
      function()->nameWithArgument(buffer, bufferSize);
    } else {
      assert(cell == &m_derivativeNumberCell);
      function()->derivativeNameWithArgument(buffer, bufferSize);
    }
    parameterCells[index]->setLabelText(buffer);
    ExplicitFloatParameterController::willDisplayCellForIndex(cell, index);
  }
}

float CurveParameterController::parameterAtIndex(int index) {
  if (isDerivative(index)) {
    assert(function()->canDisplayDerivative());
    return function()->approximateDerivative(m_cursor->x(),
                                             App::app()->localContext());
  }
  return function()->evaluateCurveParameter(index, m_cursor->t(), m_cursor->x(),
                                            m_cursor->y(),
                                            App::app()->localContext());
}

bool CurveParameterController::confirmParameterAtIndex(int parameterIndex,
                                                       double f) {
  if (function()->properties().getCurveParameter(parameterIndex).isPreimage) {
    m_preimageGraphController.setImage(f);
    return true;
  }
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth =
      (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::GetValueDisplayedOnBanner(
      f, App::app()->localContext(),
      Poincare::Preferences::sharedPreferences->numberOfSignificantDigits(),
      pixelWidth, false);

  Poincare::Coordinate2D<double> xy =
      function()->evaluateXYAtParameter(f, App::app()->localContext());
  m_cursor->moveTo(f, xy.x1(), xy.x2());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  return true;
}

bool CurveParameterController::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  int index = selectedRow();
  if (!ExplicitFloatParameterController::textFieldDidFinishEditing(
          textField, text, event)) {
    return false;
  }
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  stack->popUntilDepth(
      InteractiveCurveViewController::k_graphControllerStackDepth, true);
  if (function()->properties().getCurveParameter(index).isPreimage) {
    stack->push(&m_preimageGraphController);
  }
  return true;
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell *cell = selectedCell();
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  if (cell == &m_calculationCell &&
      m_calculationCell.ShouldEnterOnEvent(event)) {
    m_calculationParameterController.setRecord(
        m_record);  // Will select row at location 0
    stack->push(&m_calculationParameterController);
    return true;
  }
  if (cell == &m_optionsCell && m_optionsCell.ShouldEnterOnEvent(event)) {
    Shared::ListParameterController *details =
        App::app()->listController()->parameterController();
    details->setRecord(m_record);  // Will select cell at location (0,0)
    stack->push(details);
    return true;
  }
  return false;
}

bool CurveParameterController::editableParameter(int index) {
  return !isDerivative(index) &&
         function()->properties().getCurveParameter(index).editable;
}

void CurveParameterController::setRecord(Ion::Storage::Record record) {
  Shared::WithRecord::setRecord(record);
  m_derivativeNumberCell.setVisible(
      shouldDisplayDerivative() ||
      function()->properties().numberOfCurveParameters() == 3);
  m_calculationCell.setVisible(shouldDisplayCalculation());
  selectCell(0);
  resetMemoization();
  m_preimageGraphController.setRecord(record);
}

void CurveParameterController::viewWillAppear() {
  m_preimageGraphController.setImage(m_cursor->y());
  /* We need to update the visibility of the derivativeCell both when the
   * function changes (in setRecord) and here since show derivative can be
   * toggled from a sub-menu of this one. */
  m_derivativeNumberCell.setVisible(
      shouldDisplayDerivative() ||
      function()->properties().numberOfCurveParameters() == 3);
  resetMemoization();
  m_selectableListView.reloadData();
  SelectableListViewController::viewWillAppear();
}

bool CurveParameterController::shouldDisplayCalculation() const {
  return function()->canDisplayDerivative();
}

bool CurveParameterController::shouldDisplayDerivative() const {
  return function()->canDisplayDerivative() &&
         m_graphController->displayDerivativeInBanner();
}

}  // namespace Graph
