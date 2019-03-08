#include "sum_graph_controller.h"
#include "function_app.h"
#include "../apps_container.h"
#include <poincare/empty_layout.h>
#include <poincare/condensed_sum_layout.h>
#include <poincare/layout_helper.h>
#include "poincare_helpers.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Poincare;

namespace Shared {

SumGraphController::SumGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, FunctionGraphView * graphView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, CodePoint sumSymbol) :
  SimpleInteractiveCurveViewController(parentResponder, range, graphView, cursor),
  m_step(Step::FirstParameter),
  m_startSum(NAN),
  m_endSum(NAN),
  m_record(),
  m_graphRange(range),
  m_graphView(graphView),
  m_legendView(this, inputEventHandlerDelegate, sumSymbol),
  m_cursorView()
{
}

void SumGraphController::viewWillAppear() {
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  m_graphView->setBannerView(&m_legendView);
  m_graphView->setCursorView(&m_cursorView);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  m_graphView->setAreaHighlightColor(false);
  m_graphView->setAreaHighlight(NAN, NAN);
  m_graphView->reload();

  m_startSum = m_cursor->x();
  m_endSum = NAN;
  m_step = Step::FirstParameter;
  reloadBannerView();
}


void SumGraphController::didEnterResponderChain(Responder * previousFirstResponder) {
  app()->setFirstResponder(m_legendView.textField());
}

bool SumGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_step != Step::FirstParameter) {
    m_step = (Step)((int)m_step-1);
    if (m_step == Step::SecondParameter) {
      app()->setFirstResponder(m_legendView.textField());
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
  FunctionApp * myApp = static_cast<FunctionApp *>(app());
  assert(!m_record.isNull());
  ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);
  double y = function->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  if (m_step == Step::FirstParameter) {
    m_startSum = m_cursor->x();
  }
  if (m_step == Step::SecondParameter) {
    m_endSum = m_cursor->x();
    m_graphView->setAreaHighlight(m_startSum, m_endSum);
  }
  m_legendView.setEditableZone(m_cursor->x());
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
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
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  return handleEnter();
}

bool SumGraphController::textFieldDidAbortEditing(TextField * textField) {
  m_legendView.setEditableZone(m_cursor->x());
  return true;
}

bool SumGraphController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    return handleEnter();
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
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
      m_graphView->setAreaHighlight(m_startSum, m_startSum);
      m_endSum = m_startSum;
    } else {
      m_graphView->setAreaHighlightColor(true);
      m_graphView->setCursorView(nullptr);
      app()->setFirstResponder(this);
    }
    m_step = (Step)((int)m_step+1);
    reloadBannerView();
  }
  return true;
}

void SumGraphController::reloadBannerView() {
  m_legendView.setLegendMessage(legendMessageAtStep(m_step), m_step);
  double result;
  Poincare::Layout functionLayout;
  if (m_step == Step::Result) {
    FunctionApp * myApp = static_cast<FunctionApp *>(app());
    assert(!m_record.isNull());
    ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);
    result = function->sumBetweenBounds(m_startSum, m_endSum, myApp->localContext());
    functionLayout = createFunctionLayout(function);
  } else {
    m_legendView.setEditableZone(m_cursor->x());
    result = NAN;
    functionLayout = Poincare::Layout();
  }
  m_legendView.setSumSymbol(m_step, m_startSum, m_endSum, result, functionLayout);
}

/* Legend View */

SumGraphController::LegendView::LegendView(SumGraphController * controller, InputEventHandlerDelegate * inputEventHandlerDelegate, CodePoint sumSymbol) :
  m_sum(0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumLayout(),
  m_legend(k_font, I18n::Message::Default, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_editableZone(controller, m_draftText, m_draftText, TextField::maxBufferSize(), inputEventHandlerDelegate, controller, false, k_font, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumSymbol(sumSymbol)
{
  m_draftText[0] = 0;
}

void SumGraphController::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::GreyMiddle);
}

KDSize SumGraphController::LegendView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, k_legendHeight);
}

void SumGraphController::LegendView::setLegendMessage(I18n::Message message, Step step) {
  m_legend.setMessage(message);
  layoutSubviews(step);
}

void SumGraphController::LegendView::setEditableZone(double d) {
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits)];
  PrintFloat::convertFloatToText<double>(d, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
 m_editableZone.setText(buffer);
}

void SumGraphController::LegendView::setSumSymbol(Step step, double start, double end, double result, Layout functionLayout) {
  assert(step == Step::Result || functionLayout.isUninitialized());
  constexpr int sigmaLength = 2;
  const CodePoint sigma[sigmaLength] = {' ', m_sumSymbol};
  if (step == Step::FirstParameter) {
    m_sumLayout = LayoutHelper::CodePointString(sigma, sigmaLength);
  } else if (step == Step::SecondParameter) {
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    m_sumLayout = CondensedSumLayout::Builder(
        LayoutHelper::CodePointString(sigma, sigmaLength),
        LayoutHelper::String(buffer, strlen(buffer), k_font),
        EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, false, k_font, false));
  } else {
    char buffer[2+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    Layout start = LayoutHelper::String(buffer, strlen(buffer), k_font);
    PrintFloat::convertFloatToText<double>(end, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    Layout end = LayoutHelper::String(buffer, strlen(buffer), k_font);
    m_sumLayout = CondensedSumLayout::Builder(
        LayoutHelper::CodePointString(sigma, sigmaLength),
        start,
        end);
    strlcpy(buffer, "= ", 3);
    PoincareHelpers::ConvertFloatToText<double>(result, buffer+2, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
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
  layoutSubviews(step);
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

void SumGraphController::LegendView::layoutSubviews() {
  layoutSubviews(Step::FirstParameter);
}

void SumGraphController::LegendView::layoutSubviews(Step step) {
  KDCoordinate width = bounds().width();
  KDCoordinate heigth = bounds().height();
  KDSize legendSize = m_legend.minimalSizeForOptimalDisplay();

  if (legendSize.width() > 0) {
    m_sum.setFrame(KDRect(0, k_symbolHeightMargin, width-legendSize.width(), m_sum.minimalSizeForOptimalDisplay().height()));
    m_legend.setFrame(KDRect(width-legendSize.width(), 0, legendSize.width(), heigth));
  } else {
    m_sum.setFrame(bounds());
    m_legend.setFrame(KDRectZero);
  }

  KDRect frame = (step == Step::Result) ? KDRectZero : KDRect(
    2 * KDFont::LargeFont->glyphSize().width(),
    k_symbolHeightMargin + k_sigmaHeight/2 - (step == Step::SecondParameter) * editableZoneHeight(),
    editableZoneWidth(), editableZoneHeight()
  );
  m_editableZone.setFrame(frame);
}

}
