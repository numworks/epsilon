#include "sum_graph_controller.h"
#include "function_app.h"
#include <poincare/empty_layout.h>
#include <poincare/condensed_sum_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>
#include "poincare_helpers.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Poincare;

namespace Shared {

SumGraphController::SumGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, FunctionGraphView * graphView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, CodePoint sumSymbol) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_step(Step::FirstParameter),
  m_record(),
  m_graphRange(range),
  m_graphView(graphView),
  m_legendView(this, inputEventHandlerDelegate, sumSymbol),
  m_cursorView()
{
}

void SumGraphController::viewWillAppear() {
  SimpleInteractiveCurveViewController::viewWillAppear();
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  m_graphView->setBannerView(&m_legendView);
  m_graphView->setCursorView(&m_cursorView);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  m_graphView->setAreaHighlightColor(false);
  m_graphView->setAreaHighlight(NAN, NAN);
  m_graphView->reload();

  m_step = Step::FirstParameter;
  reloadBannerView();
}

void SumGraphController::didEnterResponderChain(Responder * previousFirstResponder) {
  Container::activeApp()->setFirstResponder(m_legendView.textField());
}

bool SumGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_step != Step::FirstParameter) {
    m_step = (Step)((int)m_step-1);
    if (m_step == Step::SecondParameter) {
      Container::activeApp()->setFirstResponder(m_legendView.textField());
      m_graphView->setAreaHighlightColor(false);
      m_graphView->setCursorView(&m_cursorView);
    }
    if (m_step == Step::FirstParameter) {
      m_graphView->setAreaHighlight(NAN,NAN);
      moveCursorHorizontallyToPosition(m_startSum);
    }
    reloadBannerView();
    return true;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

bool SumGraphController::moveCursorHorizontallyToPosition(double x) {
  if (std::isnan(x)) {
    return true;
  }
  FunctionApp * myApp = FunctionApp::app();
  assert(!m_record.isNull());
  ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);

  /* TODO We would like to assert that the function is not a parametered
   * function, so we can indeed evaluate the function for parameter x. */
  double y = function->evaluateXYAtParameter(x, myApp->localContext()).x2();
  m_cursor->moveTo(x, x, y);
  if (m_step == Step::SecondParameter) {
    m_graphView->setAreaHighlight(m_startSum, m_cursor->x());
  }
  m_legendView.setEditableZone(m_cursor->x());
  m_graphRange->panToMakePointVisible(x, y, cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  m_graphView->reload();
  return true;
}

void SumGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

bool SumGraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  if ((m_step == Step::SecondParameter && floatBody < m_startSum) || !moveCursorHorizontallyToPosition(floatBody)) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  return handleEnter();
}

bool SumGraphController::handleLeftRightEvent(Ion::Events::Event event) {
  if (m_step == Step::Result) {
    return false;
  }
  const double oldPosition = m_cursor->x();
  int direction = event == Ion::Events::Left ? -1 : 1;
  double newPosition = cursorNextStep(oldPosition, direction);
  if (m_step == Step::SecondParameter && newPosition < m_startSum) {
    newPosition = m_startSum;
  }
  return moveCursorHorizontallyToPosition(newPosition);
}

bool SumGraphController::handleEnter() {
  if (m_step == Step::Result) {
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->pop();
  } else {
    if (m_step == Step::FirstParameter) {
      m_startSum = m_cursor->x();
      m_graphView->setAreaHighlight(m_startSum, m_startSum);
    } else {
      m_graphView->setAreaHighlightColor(true);
      m_graphView->setCursorView(nullptr);
      Container::activeApp()->setFirstResponder(this);
    }
    m_step = (Step)((int)m_step+1);
    reloadBannerView();
  }
  return true;
}

void SumGraphController::reloadBannerView() {
  m_legendView.setLegendMessage(legendMessageAtStep(m_step), m_step);
  double endSum = NAN;
  double result;
  Poincare::Layout functionLayout;
  if (m_step == Step::Result) {
    endSum = m_cursor->x();
    FunctionApp * myApp = FunctionApp::app();
    assert(!m_record.isNull());
    ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);
    Poincare::Context * context = myApp->localContext();
    Poincare::Expression sum = function->sumBetweenBounds(m_startSum, endSum, context);
    result = PoincareHelpers::ApproximateToScalar<double>(sum, context);
    functionLayout = createFunctionLayout(function);
  } else {
    m_legendView.setEditableZone(m_cursor->x());
    result = NAN;
  }
  m_legendView.setSumSymbol(m_step, m_startSum, endSum, result, functionLayout);
}

/* Legend View */

SumGraphController::LegendView::LegendView(SumGraphController * controller, InputEventHandlerDelegate * inputEventHandlerDelegate, CodePoint sumSymbol) :
  m_sum(0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumLayout(),
  m_legend(k_font, I18n::Message::Default, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_editableZone(controller, m_textBuffer, k_editableZoneBufferSize, TextField::maxBufferSize(), inputEventHandlerDelegate, controller, k_font, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumSymbol(sumSymbol)
{
  m_textBuffer[0] = 0;
}

void SumGraphController::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::GreyMiddle);
}

KDSize SumGraphController::LegendView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, k_legendHeight);
}

void SumGraphController::LegendView::setLegendMessage(I18n::Message message, Step step) {
  m_legend.setMessage(message);
  layoutSubviews(step, false);
}

void SumGraphController::LegendView::setEditableZone(double d) {
  constexpr int precision = Preferences::MediumNumberOfSignificantDigits;
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(d, buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
  m_editableZone.setText(buffer);
}

void SumGraphController::LegendView::setSumSymbol(Step step, double start, double end, double result, Layout functionLayout) {
  assert(step == Step::Result || functionLayout.isUninitialized());
  constexpr int sigmaLength = 2;
  const CodePoint sigma[sigmaLength] = {' ', m_sumSymbol};
  if (step == Step::FirstParameter) {
    m_sumLayout = LayoutHelper::CodePointString(sigma, sigmaLength);
  } else if (step == Step::SecondParameter) {
    constexpr int precision = Preferences::MediumNumberOfSignificantDigits;
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(start, buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
    m_sumLayout = CondensedSumLayout::Builder(
        LayoutHelper::CodePointString(sigma, sigmaLength),
        LayoutHelper::String(buffer, strlen(buffer), k_font),
        EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, false, k_font, false));
  } else {
    constexpr int precision = Preferences::LargeNumberOfSignificantDigits;
    constexpr int sizeForPrecision = PrintFloat::charSizeForFloatsWithPrecision(precision);
    constexpr int bufferSize = 2 + sizeForPrecision;
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(start, buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
    Layout start = LayoutHelper::String(buffer, strlen(buffer), k_font);
    PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(end, buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
    Layout end = LayoutHelper::String(buffer, strlen(buffer), k_font);
    m_sumLayout = CondensedSumLayout::Builder(
        LayoutHelper::CodePointString(sigma, sigmaLength),
        start,
        end);
    strlcpy(buffer, "= ", 3);
    PoincareHelpers::ConvertFloatToText<double>(result, buffer+2, bufferSize-2, precision);
    m_sumLayout = HorizontalLayout::Builder(
        m_sumLayout,
        functionLayout,
        LayoutHelper::String(buffer, strlen(buffer), k_font));
  }
  m_sum.setLayout(m_sumLayout);
  if (step == Step::Result) {
    m_sum.setAlignment(0.5f, 0.5f);
  } else {
    m_sum.setAlignment(0.0f, 0.5f);
  }
  layoutSubviews(step, false);
}

View * SumGraphController::LegendView::subviewAtIndex(int index) {
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
  KDSize legendSize = m_legend.minimalSizeForOptimalDisplay();

  if (legendSize.width() > 0) {
    m_sum.setFrame(KDRect(0, k_symbolHeightMargin, width-legendSize.width(), m_sum.minimalSizeForOptimalDisplay().height()), force);
    m_legend.setFrame(KDRect(width-legendSize.width(), 0, legendSize.width(), heigth), force);
  } else {
    m_sum.setFrame(bounds(), force);
    m_legend.setFrame(KDRectZero, force);
  }

  KDRect frame = (step == Step::Result) ? KDRectZero : KDRect(
    2 * KDFont::LargeFont->glyphSize().width(),
    k_symbolHeightMargin + k_sigmaHeight/2 - (step == Step::SecondParameter) * editableZoneHeight(),
    editableZoneWidth(), editableZoneHeight()
  );
  m_editableZone.setFrame(frame, force);
}

}
