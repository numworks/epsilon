#include "curve_parameter_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/print.h>

#include "../app.h"
#include "../shared/function_name_helper.h"
#include "apps/shared/color_names.h"
#include "graph_controller.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

CurveParameterController::CurveParameterController(
    InteractiveCurveViewRange *graphRange, BannerView *bannerView,
    CurveViewCursor *cursor, GraphView *graphView,
    GraphController *graphController)
    : ExplicitFloatParameterController(parentResponder()),
      m_graphRange(graphRange),
      m_cursor(cursor),
      m_preimageGraphController(nullptr, graphView, bannerView, graphRange,
                                cursor),
      m_calculationParameterController(this, graphView, bannerView, graphRange,
                                       cursor),
      m_graphController(graphController) {
  for (int i = 0; i < k_numberOfParameterRows; i++) {
    m_parameterCells[i].setParentResponder(&m_selectableListView);
    m_parameterCells[i].setDelegate(this);
    m_parameterCells[i].setEditable(false);
  }
  m_calculationCell.label()->setMessage(I18n::Message::Find);
  m_optionsCell.label()->setMessage(I18n::Message::Options);
}

Escher::HighlightCell *CurveParameterController::cell(int row) {
  assert(0 <= row && row < k_numberOfRows);
  if (row < k_numberOfParameterRows) {
    return &m_parameterCells[row];
  }
  HighlightCell *cells[k_numberOfRows - k_numberOfParameterRows] = {
      &m_calculationCell, &m_optionsCell};
  return cells[row - k_numberOfParameterRows];
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

void CurveParameterController::fillParameterCellAtRow(int row) {
  if (row >= k_numberOfParameterRows) {
    return;
  }
  ContinuousFunctionProperties properties = function()->properties();
  if (row < properties.numberOfCurveParameters()) {
    m_parameterCells[row].setEditable(
        properties.getCurveParameter(row).editable);
  }
  constexpr size_t bufferSize =
      Escher::OneLineBufferTextView<KDFont::Size::Large>::MaxTextSize();
  char buffer[bufferSize];
  if (row == k_indexOfAbscissaCell) {
    SerializationHelper::CodePoint(buffer, bufferSize, properties.symbol());
  } else {
    assert(row == k_indexOfImageCell || row == k_indexOfDerivativeCell);
    if (properties.isParametric()) {
      FunctionNameHelper::ParametricComponentNameWithArgument(
          function().pointer(), buffer, bufferSize, row == k_indexOfImageCell);
    } else {
      if (row == k_indexOfImageCell) {
        function()->nameWithArgument(buffer, bufferSize);
      } else {
        assert(row == k_indexOfDerivativeCell);
        function()->derivativeNameWithArgument(buffer, bufferSize);
      }
    }
  }
  m_parameterCells[row].label()->setText(buffer);
  ExplicitFloatParameterController::fillParameterCellAtRow(row);
}

double CurveParameterController::parameterAtIndex(int index) {
  Poincare::Context *ctx = App::app()->localContext();
  if (isDerivative(index)) {
    assert(function()->canDisplayDerivative());
    return function()->approximateDerivative(m_cursor->t(), ctx);
  }
  double t = m_cursor->t();
  double x = m_cursor->x();
  double y = m_cursor->y();
  if (function()->properties().isScatterPlot() &&
      (t != std::round(t) ||
       t >= function()->iterateScatterPlot(ctx).length())) {
    /* FIXME This will display the first point of a multi-point scatter plot
     * when accessed through the Calculate button, which is not super useful,
     * but there is no real alternative barring some UX changes. */
    t = 0.;
    Poincare::Coordinate2D<double> xy =
        function()->evaluateXYAtParameter(t, ctx);
    x = xy.x();
    y = xy.y();
  }
  return function()->evaluateCurveParameter(index, t, x, y, ctx);
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

  m_graphRange->setZoomAuto(false);
  m_graphController->moveCursorAndCenterIfNeeded(f);

  return true;
}

bool CurveParameterController::textFieldDidFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  int index = selectedRow();
  if (!ExplicitFloatParameterController::textFieldDidFinishEditing(textField,
                                                                   event)) {
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

TextField *CurveParameterController::textFieldOfCellAtRow(int row) {
  assert(0 <= row && row <= k_numberOfParameterRows);
  return static_cast<ParameterCell *>(cell(row))->textField();
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell *cell = selectedCell();
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  if (cell == &m_calculationCell &&
      m_calculationCell.canBeActivatedByEvent(event)) {
    m_calculationParameterController.setRecord(
        m_record);  // Will select row at location 0
    stack->push(&m_calculationParameterController);
    return true;
  }
  if (cell == &m_optionsCell && m_optionsCell.canBeActivatedByEvent(event)) {
    FunctionParameterController *details = App::app()->parameterController();
    details->setUseColumnTitle(false);
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
  updateNumberOfParameterCells();
  m_calculationCell.setVisible(shouldDisplayCalculation());
  selectRow(0);
  m_selectableListView.resetSizeAndOffsetMemoization();
  m_preimageGraphController.setRecord(record);
}

void CurveParameterController::viewWillAppear() {
  m_preimageGraphController.setImage(m_cursor->y());
  /* We need to update the visibility of the derivativeCell both when the
   * function changes (in setRecord) and here since show derivative can be
   * toggled from a sub-menu of this one. */
  updateNumberOfParameterCells();
  ExplicitFloatParameterController::viewWillAppear();
}

bool CurveParameterController::shouldDisplayCalculation() const {
  return function()->canCalculateOnCurve();
}

bool CurveParameterController::shouldDisplayDerivative() const {
  return function()->displayDerivative();
}

void CurveParameterController::didBecomeFirstResponder() {
  if (!function()->isActive()) {
    static_cast<StackViewController *>(parentResponder())
        ->popUntilDepth(
            Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
            true);
    return;
  }
  Shared::ExplicitFloatParameterController::didBecomeFirstResponder();
}

void CurveParameterController::updateNumberOfParameterCells() {
  m_parameterCells[k_indexOfDerivativeCell].setVisible(
      shouldDisplayDerivative() || function()->properties().isParametric());
}

}  // namespace Graph
