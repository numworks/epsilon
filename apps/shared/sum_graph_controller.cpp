#include "sum_graph_controller.h"

#include <assert.h>
#include <escher/clipboard.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/print.h>
#include <stdlib.h>

#include <cmath>

#include "function_app.h"
#include "poincare_helpers.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

SumGraphController::SumGraphController(Responder *parentResponder,
                                       FunctionGraphView *graphView,
                                       InteractiveCurveViewRange *range,
                                       CurveViewCursor *cursor)
    : SimpleInteractiveCurveViewController(parentResponder, cursor),
      m_step(Step::FirstParameter),
      m_startSum(NAN),
      m_result(NAN),
      m_graphRange(range),
      m_graphView(graphView),
      m_legendView(this) {}

void SumGraphController::viewWillAppear() {
  panToMakeCursorVisible();
  m_graphView->setBannerView(&m_legendView);
  m_graphView->setCursorView(&m_cursorView);
  m_graphView->setFocus(true);
  m_graphView->setAreaHighlightColor(false);
  m_graphView->setAreaHighlight(NAN, NAN);
  m_step = Step::FirstParameter;
  makeCursorVisibleAndReloadBanner();
  SimpleInteractiveCurveViewController::viewWillAppear();
}

void SumGraphController::didBecomeFirstResponder() {
  /* Do not set the textField as first responder when displaying the result
   * so that Copy and Sto apply on the result. */
  if (m_step != Step::Result) {
    App::app()->setFirstResponder(m_legendView.textField());
  }
}

bool SumGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_step != Step::FirstParameter) {
    m_step = (Step)((int)m_step - 1);
    if (m_step == Step::SecondParameter) {
      App::app()->setFirstResponder(m_legendView.textField());
      m_graphView->setAreaHighlightColor(false);
      m_graphView->setCursorView(&m_cursorView);
    }
    if (m_step == Step::FirstParameter) {
      m_graphView->setAreaHighlight(NAN, NAN);
      moveCursorHorizontallyToPosition(m_startSum);
    }
    reloadBannerView();
    return true;
  }
  if ((event == Ion::Events::Copy || event == Ion::Events::Cut ||
       event == Ion::Events::Sto || event == Ion::Events::Var) &&
      m_step == Step::Result) {
    /* We want to save more digits than we have in the banner to we need to
     * convert the result here */
    constexpr static int precision =
        Preferences::DefaultNumberOfPrintedSignificantDigits;
    constexpr static int bufferSize =
        PrintFloat::charSizeForFloatsWithPrecision(precision);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(m_result, buffer, bufferSize,
                                                precision);
    if (event == Ion::Events::Sto || event == Ion::Events::Var) {
      App::app()->storeValue(buffer);
    } else {
      Escher::Clipboard::SharedClipboard()->store(buffer);
    }
    return true;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

bool SumGraphController::moveCursorHorizontallyToPosition(double x) {
  if (std::isnan(x)) {
    return true;
  }
  /* m_cursorView is a vertical bar so no need to compute its ordinate.
   *
   * TODO We would like to assert that the function is not a parametered
   * function, so we can indeed evaluate the function for parameter x. */
  m_cursor->moveTo(x, x, 0);
  if (m_step == Step::SecondParameter) {
    assert(allowEndLowerThanStart() || m_cursor->x() >= m_startSum);
    m_graphView->setAreaHighlight(std::min(m_startSum, m_cursor->x()),
                                  std::max(m_startSum, m_cursor->x()));
  }
  m_legendView.setEditableZone(m_cursor->x());
  makeCursorVisibleAndReloadBanner();
  m_graphView->reload(true);
  return true;
}

void SumGraphController::makeCursorVisibleAndReloadBanner() {
  makeCursorVisible();
  reloadBannerView();
}

void SumGraphController::makeCursorVisible() {
  float position = m_cursor->x();
  if (!std::isfinite(position)) {
    return;
  }
  ExpiringPointer<Function> function =
      FunctionApp::app()->functionStore()->modelForRecord(selectedRecord());
  float y =
      function
          ->evaluateXYAtParameter(position, FunctionApp::app()->localContext())
          .y();
  // Do not zoom out if user is selecting first parameter
  makeDotVisible(position, y, m_step != Step::FirstParameter);
  makeCursorVisibleOnSecondCurve(position);
}

void SumGraphController::makeCursorVisibleOnSecondCurve(float x) {
  // zoomOut is always true so that the user can see both dots
  makeDotVisible(x, 0.0, true);
}

void SumGraphController::makeDotVisible(float x, float y, bool zoomOut) {
  if (zoomOut) {
    m_graphRange->zoomOutToMakePointVisible(
        x, y, cursorTopMarginRatio(), cursorRightMarginRatio(),
        cursorBottomMarginRatio(), cursorLeftMarginRatio());
  } else {
    m_graphRange->panToMakePointVisible(
        x, y, cursorTopMarginRatio(), cursorRightMarginRatio(),
        cursorBottomMarginRatio(), cursorLeftMarginRatio(),
        curveView()->pixelWidth());
  }
}

void SumGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
}

bool SumGraphController::textFieldDidFinishEditing(AbstractTextField *textField,
                                                   Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  if ((!allowEndLowerThanStart() && m_step == Step::SecondParameter &&
       floatBody < m_startSum) ||
      !moveCursorHorizontallyToPosition(floatBody)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  return handleEnter();
}

bool SumGraphController::handleLeftRightEvent(Ion::Events::Event event) {
  if (m_step == Step::Result) {
    return false;
  }
  const double oldPosition = m_cursor->x();
  double newPosition = cursorNextStep(oldPosition, OMG::Direction(event));
  if (!allowEndLowerThanStart() && m_step == Step::SecondParameter &&
      newPosition < m_startSum) {
    newPosition = m_startSum;
  }
  return moveCursorHorizontallyToPosition(newPosition);
}

bool SumGraphController::handleEnter() {
  if (m_step == Step::Result) {
    StackViewController *stack = (StackViewController *)parentResponder();
    stack->pop();
  } else {
    Step currentStep = m_step;
    /* Set the step now so that setFirstResponder know it does not need to set
     * the textField as first responder. */
    m_step = (Step)((int)m_step + 1);
    if (currentStep == Step::FirstParameter) {
      m_startSum = m_cursor->x();
      m_graphView->setAreaHighlight(m_startSum, m_startSum);
    } else {
      m_graphView->setAreaHighlightColor(true);
      m_graphView->setCursorView(nullptr);
      App::app()->setFirstResponder(this);
    }
    reloadBannerView();
  }
  return true;
}

void SumGraphController::reloadBannerView() {
  m_legendView.setLegendMessage(legendMessageAtStep(m_step), m_step);
  double endSum = NAN;
  double result;
  Layout functionLayout;
  if (m_step == Step::Result) {
    endSum = m_cursor->x();
    assert(!selectedRecord().isNull());
    Poincare::Context *context = FunctionApp::app()->localContext();
    Poincare::Expression sum = createSumExpression(m_startSum, endSum, context);
    result = PoincareHelpers::ApproximateToScalar<double>(sum, context);
    functionLayout = createFunctionLayout();
  } else {
    m_legendView.setEditableZone(m_cursor->x());
    result = NAN;
  }
  m_result = result;
  m_legendView.setSumLayout(m_step, m_startSum, endSum, result, functionLayout,
                            sumSymbol());
}

Poincare::Expression SumGraphController::createSumExpression(
    double startSum, double endSum, Poincare::Context *context) {
  ExpiringPointer<Function> function =
      FunctionApp::app()->functionStore()->modelForRecord(selectedRecord());
  return function->sumBetweenBounds(startSum, endSum, context);
}

/* Legend View */

SumGraphController::LegendView::LegendView(SumGraphController *controller)
    : m_sum(k_glyphsFormat),
      m_legend(I18n::Message::Default, k_glyphsFormat),
      m_editableZone(controller, m_textBuffer, k_editableZoneBufferSize,
                     controller, k_glyphsFormat) {
  m_textBuffer[0] = 0;
}

void SumGraphController::LegendView::drawRect(KDContext *ctx,
                                              KDRect rect) const {
  ctx->fillRect(bounds(), Palette::GrayMiddle);
}

KDSize SumGraphController::LegendView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, k_legendHeight);
}

void SumGraphController::LegendView::setLegendMessage(I18n::Message message,
                                                      Step step) {
  m_legend.setMessage(message);
  layoutSubviews(step, false);
}

void SumGraphController::LegendView::setEditableZone(double d) {
  char buffer[k_valuesBufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
      d, buffer, k_valuesBufferSize, k_valuesPrecision,
      Preferences::PrintFloatMode::Decimal);
  m_editableZone.setText(buffer);
}

static Layout emptyValueLayout() { return " "_l; }

static Layout valueLayout(double value, int numberOfSignificantDigits,
                          Preferences::PrintFloatMode displayMode) {
  constexpr static int k_bufferSize = Poincare::PrintFloat::k_maxFloatCharSize;
  char buffer[k_bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
      value, buffer, k_bufferSize, numberOfSignificantDigits, displayMode);
  Layout layout = Layout::String(buffer, strlen(buffer));
  return layout;
}

static Layout areaMessageLayout() {
  constexpr static int bufferSize = sizeof("Oppervlakte") + 1;
  char buffer[bufferSize];
  int length = Print::CustomPrintf(buffer, bufferSize, "%s",
                                   I18n::translate(I18n::Message::Area));
  return Layout::String(buffer, length);
}

void SumGraphController::LegendView::setSumLayout(Step step, double start,
                                                  double end, double result,
                                                  Layout functionLayout,
                                                  CodePoint sumSymbol) {
  assert(!std::isnan(start) || step == Step::FirstParameter);
  assert(!std::isnan(end) || step != Step::Result);
  Layout startLayout =
      valueLayout(start, k_valuesPrecision, k_valuesDisplayMode);
  Layout endLayout =
      step == Step::FirstParameter
          ? emptyValueLayout()
          : valueLayout(end, k_valuesPrecision, k_valuesDisplayMode);
  Layout sumLayout = Layout::Create(
      KCondensedSumL(KA, KB, KC),
      {.KA = Layout::CodePoint(sumSymbol), .KB = startLayout, .KC = endLayout});
  if (step == Step::Result) {
    Layout leftLayout;
    Layout equalLayout = Layout::String(" = ", 3);
    Preferences *preferences = Preferences::SharedPreferences();
    Layout resultLayout =
        valueLayout(result, preferences->numberOfSignificantDigits(),
                    preferences->displayMode());
    if (functionLayout.isUninitialized() ||
        (sumLayout->layoutSize(k_font).width() +
             functionLayout->layoutSize(k_font).width() +
             equalLayout->layoutSize(k_font).width() +
             resultLayout->layoutSize(k_font).width() >
         bounds().width())) {
      assert(sumSymbol == UCodePointIntegral);
      leftLayout = areaMessageLayout();
    } else {
      leftLayout =
          Layout::Create(KA ^ KB, {.KA = sumLayout, .KB = functionLayout});
    }

    sumLayout = leftLayout = Layout::Create(
        KA ^ KB ^ KC,
        {.KA = leftLayout, .KB = equalLayout, .KC = resultLayout});
  }
  m_sum.setLayout(sumLayout);
  m_sum.setAlignment(
      step == Step::Result ? KDGlyph::k_alignCenter : KDGlyph::k_alignLeft,
      KDGlyph::k_alignCenter);
  layoutSubviews(step, false);
}

View *SumGraphController::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return &m_sum;
  }
  if (index == 1) {
    return &m_editableZone;
  }
  return &m_legend;
}

void SumGraphController::LegendView::layoutSubviews(bool force) {
  layoutSubviews(Step::FirstParameter, force);
}

void SumGraphController::LegendView::layoutSubviews(Step step, bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate heigth = bounds().height();
  KDCoordinate legendWidth = m_legend.minimalSizeForOptimalDisplay().width();
  constexpr static KDCoordinate horizontalMargin = 7;

  KDRect sumFrame = bounds();
  KDRect legendFrame = KDRectZero;
  if (legendWidth > 0) {
    sumFrame = KDRect(horizontalMargin, 0, width - legendWidth, heigth);
    legendFrame =
        KDRect(width - legendWidth - horizontalMargin, 0, legendWidth, heigth);
  }
  setChildFrame(&m_sum, sumFrame, force);
  setChildFrame(&m_legend, legendFrame, force);

  KDRect editableZoneFrame = KDRectZero;
  if (step != Step::Result) {
    constexpr static KDCoordinate sumSymbolWidth = KDFont::GlyphWidth(k_font);
    constexpr static KDCoordinate editableZoneWidth =
        12 * KDFont::GlyphWidth(k_font);
    constexpr static KDCoordinate editableZoneHeight =
        KDFont::GlyphHeight(k_font);
    editableZoneFrame = KDRect(
        horizontalMargin + sumSymbolWidth,
        heigth / 2 - (step == Step::SecondParameter) * editableZoneHeight,
        editableZoneWidth, editableZoneHeight);
  }
  setChildFrame(&m_editableZone, editableZoneFrame, force);
}

}  // namespace Shared
