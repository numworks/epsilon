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
    InteractiveCurveViewRange *graphRange, BannerView *bannerView,
    CurveViewCursor *cursor, GraphView *graphView,
    GraphController *graphController)
    : ExplicitFloatParameterController(parentResponder()),
      m_abscissaCell(&m_selectableListView, this),
      m_imageCell(&m_selectableListView, this),
      m_derivativeNumberCell(&m_selectableListView, this),
      m_graphRange(graphRange),
      m_cursor(cursor),
      m_preimageGraphController(nullptr, graphView, bannerView, graphRange,
                                cursor),
      m_calculationParameterController(this, graphView, bannerView, graphRange,
                                       cursor),
      m_graphController(graphController) {
  m_calculationCell.label()->setMessage(I18n::Message::Find);
  m_optionsCell.label()->setMessage(I18n::Message::Options);
  m_derivativeNumberCell.setEditable(false);
}

Escher::HighlightCell *CurveParameterController::cell(int index) {
  assert(0 <= index && index < k_numberOfRows);
  HighlightCell *cells[k_numberOfRows] = {&m_abscissaCell, &m_imageCell,
                                          &m_derivativeNumberCell,
                                          &m_calculationCell, &m_optionsCell};
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

void CurveParameterController::fillParameterCellAtRow(int row) {
  I18n::Message name = I18n::Message::Default;
  MenuCellWithEditableText<OneLineBufferTextView<KDFont::Size::Large>>
      *parameterCells[] = {&m_abscissaCell, &m_imageCell,
                           &m_derivativeNumberCell};
  if (row < function()->properties().numberOfCurveParameters()) {
    ContinuousFunctionProperties::CurveParameter parameter =
        function()->properties().getCurveParameter(row);
    name = parameter.parameterName;
    parameterCells[row]->setEditable(parameter.editable);
  }
  if (name != I18n::Message::Default) {
    parameterCells[row]->label()->setMessageWithPlaceholders(name);
    ExplicitFloatParameterController::fillParameterCellAtRow(row);
    return;
  }
  if (cell(row) == &m_imageCell || cell(row) == &m_derivativeNumberCell) {
    // The parameter requires a custom name built from the function name
    constexpr size_t bufferSize =
        Escher::OneLineBufferTextView<KDFont::Size::Large>::MaxTextSize();
    char buffer[bufferSize];
    if (cell(row) == &m_imageCell) {
      function()->nameWithArgument(buffer, bufferSize);
    } else {
      assert(cell(row) == &m_derivativeNumberCell);
      function()->derivativeNameWithArgument(buffer, bufferSize);
    }
    parameterCells[row]->label()->setText(buffer);
    ExplicitFloatParameterController::fillParameterCellAtRow(row);
  }
}

double CurveParameterController::parameterAtIndex(int index) {
  Poincare::Context *ctx = App::app()->localContext();
  if (isDerivative(index)) {
    assert(function()->canDisplayDerivative());
    return function()->approximateDerivative(m_cursor->x(), ctx);
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
  assert(cell(row) == &m_abscissaCell || cell(row) == &m_imageCell ||
         cell(row) == &m_derivativeNumberCell);
  return static_cast<MenuCellWithEditableText<
      OneLineBufferTextView<KDFont::Size::Large>> *>(cell(row))
      ->textField();
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
  m_derivativeNumberCell.setVisible(
      shouldDisplayDerivative() ||
      function()->properties().numberOfCurveParameters() == 3);
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
  m_derivativeNumberCell.setVisible(
      shouldDisplayDerivative() ||
      function()->properties().numberOfCurveParameters() == 3);
  ExplicitFloatParameterController::viewWillAppear();
}

bool CurveParameterController::shouldDisplayCalculation() const {
  return function()->canDisplayDerivative();
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

}  // namespace Graph
