#include "curve_parameter_controller.h"

#include <apps/i18n.h>
#include <apps/shared/function_name_helper.h>
#include <assert.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/print.h>

#include "../app.h"
#include "apps/shared/color_names.h"
#include "graph_controller.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

CurveParameterController::CurveParameterController(
    InteractiveCurveViewRange* graphRange, BannerView* bannerView,
    CurveViewCursor* cursor, GraphView* graphView,
    GraphController* graphController,
    FunctionParameterController* functionParameterController,
    DerivativeColumnParameterController* derivativeColumnParameterController)
    : ExplicitFloatParameterController(nullptr),
      m_graphRange(graphRange),
      m_cursor(cursor),
      m_preimageGraphController(nullptr, graphView, bannerView, graphRange,
                                cursor),
      m_calculationParameterController(nullptr, graphView, bannerView,
                                       graphRange, cursor),
      m_functionParameterController(functionParameterController),
      m_derivativeColumnParameterController(
          derivativeColumnParameterController),
      m_derivationOrder(0),
      m_graphController(graphController) {
  for (int i = 0; i < k_numberOfParameterRows; i++) {
    m_parameterCells[i].setParentResponder(&m_selectableListView);
    m_parameterCells[i].setDelegate(this);
    m_parameterCells[i].setEditable(false);
  }
  m_calculationCell.label()->setMessage(I18n::Message::Find);
  m_optionsCell.label()->setMessage(I18n::Message::Options);
}

Escher::HighlightCell* CurveParameterController::cell(int row) {
  assert(0 <= row && row < k_numberOfRows);
  if (row < k_numberOfParameterRows) {
    return &m_parameterCells[row];
  }
  HighlightCell* cells[k_numberOfRows - k_numberOfParameterRows] = {
      &m_calculationCell, &m_optionsCell};
  return cells[row - k_numberOfParameterRows];
}

Shared::ExpiringPointer<Shared::ContinuousFunction>
CurveParameterController::function() const {
  return App::app()->functionStore()->modelForRecord(m_record);
}

const char* CurveParameterController::title() {
  if (function()->isNamed()) {
    const char* calculate = I18n::translate(I18n::Message::CalculateOnFx);
    size_t len = strlen(calculate);
    memcpy(m_title, calculate, len);
    function()->nameWithArgument(m_title + len, k_titleSize - len,
                                 m_derivationOrder);
  } else {
    const char* colorName = I18n::translate(
        Shared::ColorNames::NameForCurveColor(function()->color()));
    Poincare::Print::CustomPrintf(
        m_title, k_titleSize,
        I18n::translate(I18n::Message::CalculateOnTheCurve), colorName);
  }
  return m_title;
}

bool CurveParameterController::parameterAtRowIsFirstComponent(int row) const {
  assert(0 <= row && row <= k_numberOfParameterRows);
  ParameterIndex index = static_cast<ParameterIndex>(row);
  switch (index) {
    case ParameterIndex::Image1:
    case ParameterIndex::FirstDerivative1:
    case ParameterIndex::SecondDerivative1:
      return true;
    default:
      assert(index == ParameterIndex::Image2 ||
             index == ParameterIndex::FirstDerivative2 ||
             index == ParameterIndex::SecondDerivative2);
      return false;
  }
}

int CurveParameterController::derivationOrderOfParameterAtRow(int row) const {
  assert(0 <= row && row <= k_numberOfParameterRows);
  ParameterIndex index = static_cast<ParameterIndex>(row);
  switch (index) {
    case ParameterIndex::Abscissa:
      return -1;
    case ParameterIndex::Image1:
    case ParameterIndex::Image2:
      return 0;
    case ParameterIndex::FirstDerivative1:
    case ParameterIndex::FirstDerivative2:
      return 1;
    default:
      assert(index == ParameterIndex::SecondDerivative1 ||
             index == ParameterIndex::SecondDerivative2);
      return 2;
  }
}

void CurveParameterController::fillParameterCellAtRow(int row) {
  if (row >= k_numberOfParameterRows) {
    return;
  }
  ContinuousFunctionProperties properties = function()->properties();
  if (row < properties.numberOfCurveParameters()) {
    m_parameterCells[row].setEditable(
        properties.parameterAtIndexIsEditable(row));
  }
  constexpr size_t bufferSize =
      Escher::OneLineBufferTextView<KDFont::Size::Large>::MaxTextSize();
  char buffer[bufferSize];
  if (row == static_cast<int>(ParameterIndex::Abscissa)) {
    SerializationHelper::CodePoint(buffer, bufferSize, properties.symbol());
  } else {
    bool firstComponent = parameterAtRowIsFirstComponent(row);
    int derivationOrder = derivationOrderOfParameterAtRow(row);
    if (properties.isParametric()) {
      FunctionNameHelper::ParametricComponentNameWithArgument(
          function().pointer(), buffer, bufferSize, firstComponent,
          derivationOrder);
    } else {
      assert(firstComponent);
      function()->nameWithArgument(buffer, bufferSize, derivationOrder);
    }
  }
  m_parameterCells[row].label()->setText(buffer);
  ExplicitFloatParameterController::fillParameterCellAtRow(row);
}

double CurveParameterController::parameterAtIndex(int index) {
  Poincare::Context* ctx = App::app()->localContext();
  int derivationOrder = derivationOrderOfParameterAtRow(index);
  if (derivationOrder >= 1) {
    assert(derivationOrder == 1 || derivationOrder == 2);
    assert(function()->canDisplayDerivative());
    bool firstComponent = parameterAtRowIsFirstComponent(index);
    PointOrScalar<double> derivative =
        function()->approximateDerivative<double>(m_cursor->t(), ctx,
                                                  derivationOrder);
    if (derivative.isScalar()) {
      assert(firstComponent);
      return derivative.toScalar();
    }
    assert(derivative.isPoint());
    Coordinate2D<double> xy = derivative.toPoint();
    return firstComponent ? xy.x() : xy.y();
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
  if (function()->properties().parameterAtIndexIsPreimage(parameterIndex)) {
    m_preimageGraphController.setImage(f);
    return true;
  }
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth =
      (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::GetValueDisplayedOnBanner(
      f, App::app()->localContext(),
      Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits(),
      pixelWidth, false);

  m_graphRange->setZoomAuto(false);
  m_graphController->moveCursorAndCenterIfNeeded(f);

  return true;
}

bool CurveParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  int index = selectedRow();
  if (!ExplicitFloatParameterController::textFieldDidFinishEditing(textField,
                                                                   event)) {
    return false;
  }
  StackViewController* stack = stackController();
  stack->popUntilDepth(
      InteractiveCurveViewController::k_graphControllerStackDepth, true);
  if (function()->properties().parameterAtIndexIsPreimage(index)) {
    stack->push(&m_preimageGraphController);
  }
  return true;
}

TextField* CurveParameterController::textFieldOfCellAtRow(int row) {
  assert(0 <= row && row <= k_numberOfParameterRows);
  return static_cast<ParameterCell*>(cell(row))->textField();
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  StackViewController* stack = stackController();
  if (cell == &m_calculationCell &&
      m_calculationCell.canBeActivatedByEvent(event)) {
    m_calculationParameterController.setRecord(m_record);
    stack->push(&m_calculationParameterController);
    return true;
  }
  if (cell == &m_optionsCell && m_optionsCell.canBeActivatedByEvent(event)) {
    if (m_derivationOrder == 0) {
      m_functionParameterController->setRecord(m_record);
      m_functionParameterController->setParameterDelegate(this);
      stackController()->push(m_functionParameterController);
    } else {
      m_derivativeColumnParameterController->setRecord(m_record,
                                                       m_derivationOrder);
      m_derivativeColumnParameterController->setParameterDelegate(this);
      stackController()->push(m_derivativeColumnParameterController);
    }
    return true;
  }
  return false;
}

void CurveParameterController::setRecord(Ion::Storage::Record record,
                                         int derivationOrder) {
  m_record = record;
  m_derivationOrder = derivationOrder;
  m_calculationCell.setVisible(function()->canDisplayDerivative() &&
                               m_derivationOrder == 0);
  selectRow(0);
  m_selectableListView.resetSizeAndOffsetMemoization();
  m_preimageGraphController.setRecord(record);
}

void CurveParameterController::hideDerivative(Ion::Storage::Record record,
                                              int derivationOrder) {
  if (derivationOrder == 1) {
    function()->setDisplayPlotFirstDerivative(false);
  } else {
    assert(derivationOrder == 2);
    function()->setDisplayPlotSecondDerivative(false);
  }
  stackController()->popUntilDepth(
      Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
      true);
}

void CurveParameterController::viewWillAppear() {
  m_preimageGraphController.setImage(m_cursor->y());
  /* We need to update the visibility of the derivativeCell here (and not in
   * setRecord) in since show derivative can be toggled from a sub-menu of
   * this one. */
  bool isParametric = function()->properties().isParametric();
  bool displayImage, displayValueFirstDerivative, displayValueSecondDerivative;
  function()->valuesToDisplayOnDerivativeCurve(m_derivationOrder, &displayImage,
                                               &displayValueFirstDerivative,
                                               &displayValueSecondDerivative);
  parameterCell(ParameterIndex::Image1)->setVisible(displayImage);
  parameterCell(ParameterIndex::Image2)
      ->setVisible(isParametric && displayImage);
  parameterCell(ParameterIndex::FirstDerivative1)
      ->setVisible(displayValueFirstDerivative);
  parameterCell(ParameterIndex::FirstDerivative2)
      ->setVisible(isParametric && displayValueFirstDerivative);
  parameterCell(ParameterIndex::SecondDerivative1)
      ->setVisible(displayValueSecondDerivative);
  parameterCell(ParameterIndex::SecondDerivative2)
      ->setVisible(isParametric && displayValueSecondDerivative);
  ExplicitFloatParameterController::viewWillAppear();
}

void CurveParameterController::didBecomeFirstResponder() {
  if (!function()->isActive()) {
    stackController()->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        true);
    return;
  }
  Shared::ExplicitFloatParameterController::didBecomeFirstResponder();
}

StackViewController* CurveParameterController::stackController() const {
  return static_cast<StackViewController*>(parentResponder());
}

}  // namespace Graph
