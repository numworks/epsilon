#include "sum_graph_controller.h"
#include "../apps_container.h"
#include <poincare/layout_helper.h>
#include "poincare_helpers.h"
#include <poincare/empty_layout.h>
#include <poincare/condensed_sum_layout.h>

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
  m_function(nullptr),
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
  m_legendView.setLegendMessage(legendMessageAtStep(Step::FirstParameter), Step::FirstParameter);
  m_legendView.setEditableZone(m_startSum);
  m_legendView.setSumSymbol(m_step);
}


void SumGraphController::didEnterResponderChain(Responder * previousFirstResponder) {
  app()->setFirstResponder(m_legendView.textField());
}

bool SumGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return handleZoom(event);
  }
  if ((int)m_step > 1 && event != Ion::Events::OK && event != Ion::Events::EXE && event != Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Left) {
    if ((int)m_step > 0 && m_startSum >= m_cursor->x()) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(cursorNextStep(m_cursor->x(), -1))) {
      m_graphView->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Right) {
    if (moveCursorHorizontallyToPosition(cursorNextStep(m_cursor->x(), 1))) {
      m_graphView->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return handleEnter();
  }
  if (event == Ion::Events::Back && (int)m_step > 0) {
    m_step = (Step)((int)m_step-1);
    m_legendView.setLegendMessage(legendMessageAtStep(m_step), m_step);
    if (m_step == Step::SecondParameter) {
      app()->setFirstResponder(m_legendView.textField());
      m_graphView->setAreaHighlightColor(false);
      m_graphView->setCursorView(&m_cursorView);
      m_endSum = m_cursor->x();
      m_legendView.setEditableZone(m_endSum);
      m_legendView.setSumSymbol(m_step, m_startSum);
    }
    if (m_step == Step::FirstParameter) {
      m_graphView->setAreaHighlight(NAN,NAN);
      moveCursorHorizontallyToPosition(m_startSum);
      m_legendView.setLegendMessage(legendMessageAtStep(m_step), m_step);
      m_legendView.setEditableZone(m_startSum);
      m_legendView.setSumSymbol(m_step);
      m_graphView->reload();
    }
    return true;
  }
  return false;
}

bool SumGraphController::moveCursorHorizontallyToPosition(double x) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  if (m_function == nullptr) {
    return false;
  }
  double y = m_function->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  if (m_step == Step::FirstParameter) {
    m_startSum = m_cursor->x();
    m_legendView.setEditableZone(m_startSum);
  }
  if (m_step == Step::SecondParameter) {
    m_graphView->setAreaHighlight(m_startSum, m_cursor->x());
    m_endSum = m_cursor->x();
    m_legendView.setEditableZone(m_endSum);
  }
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

void SumGraphController::setFunction(Function * function) {
  m_graphView->selectFunction(function);
  m_function = function;
}

bool SumGraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  double floatBody = PoincareHelpers::ApproximateToScalar<double>(text, *globalContext);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (m_step == Step::SecondParameter && floatBody < m_startSum) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (moveCursorHorizontallyToPosition(floatBody)) {
    handleEnter();
    m_graphView->reload();
    return true;
  }
  app()->displayWarning(I18n::Message::ForbiddenValue);
  return false;
}

bool SumGraphController::textFieldDidAbortEditing(TextField * textField) {
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits)];
  double parameter = NAN;
  switch(m_step) {
    case Step::FirstParameter:
      parameter = m_startSum;
      break;
    case Step::SecondParameter:
      parameter = m_endSum;
      break;
    default:
      assert(false);
  }
  PrintFloat::convertFloatToText<double>(parameter, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
  textField->setText(buffer);
  return true;
}

bool SumGraphController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    return handleEnter();
  }
  if (m_step == Step::Result) {
    return handleEvent(event);
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

bool SumGraphController::handleEnter() {
  if (m_step == Step::Result) {
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->pop();
    return true;
  }
  if (m_step == Step::FirstParameter) {
    m_step = Step::SecondParameter;
    m_graphView->setAreaHighlight(m_startSum,m_startSum);
    m_endSum = m_cursor->x();
    m_legendView.setEditableZone(m_endSum);
    m_legendView.setSumSymbol(m_step, m_startSum);
    m_legendView.setLegendMessage(legendMessageAtStep(m_step), m_step);
    return true;
  }
  m_step = (Step)((int)m_step+1);
  TextFieldDelegateApp * myApp = static_cast<TextFieldDelegateApp *>(app());
  double sum = m_function->sumBetweenBounds(m_startSum, m_endSum, myApp->localContext());
  m_legendView.setSumSymbol(m_step, m_startSum, m_endSum, sum, createFunctionLayout(m_function->name()));
  m_legendView.setLegendMessage(I18n::Message::Default, m_step);
  m_graphView->setAreaHighlightColor(true);
  m_graphView->setCursorView(nullptr);
  myApp->setFirstResponder(this);
  return true;
}

/* Legend View */

SumGraphController::LegendView::LegendView(SumGraphController * controller, InputEventHandlerDelegate * inputEventHandlerDelegate, CodePoint sumSymbol) :
  m_sum(0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumLayout(),
  m_legend(KDFont::SmallFont, I18n::Message::Default, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_editableZone(controller, m_draftText, m_draftText, TextField::maxBufferSize(), inputEventHandlerDelegate, controller, false, KDFont::SmallFont, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
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
  constexpr int sigmaSize = 2;
  const CodePoint sigma[sigmaSize] = {UCodePointSpace, m_sumSymbol};
  if (step == Step::FirstParameter) {
    m_sumLayout = LayoutHelper::CodePointString(sigma, sigmaSize);
  } else if (step == Step::SecondParameter) {
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    m_sumLayout = CondensedSumLayout::Builder(
        LayoutHelper::CodePointString(sigma, sizeof(sigma)),
        LayoutHelper::String(buffer, strlen(buffer), KDFont::SmallFont),
        EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, false, KDFont::SmallFont, false));
  } else {
    m_sumLayout = LayoutHelper::CodePointString(sigma, sigmaSize);
    constexpr size_t bufferSize = 2+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    PrintFloat::convertFloatToText<double>(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    Layout start = LayoutHelper::String(buffer, strlen(buffer), KDFont::SmallFont);
    PrintFloat::convertFloatToText<double>(end, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    Layout end = LayoutHelper::String(buffer, strlen(buffer), KDFont::SmallFont);
    m_sumLayout = CondensedSumLayout::Builder(
        LayoutHelper::CodePointString(sigma, sizeof(sigma)),
        start,
        end);
    strlcpy(buffer, "= ", bufferSize);
    PoincareHelpers::ConvertFloatToText<double>(result, buffer+2, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    m_sumLayout = HorizontalLayout::Builder(
        m_sumLayout,
        functionLayout,
        LayoutHelper::String(buffer, strlen(buffer), KDFont::SmallFont));
  }
  m_sum.setLayout(m_sumLayout);
  if (step == Step::Result) {
    m_sum.setAlignment(0.5f, 0.5f);
  } else {
    m_sum.setAlignment(0.0f, 0.5f);
  }
  layoutSubviews(step);
}

int SumGraphController::LegendView::numberOfSubviews() const {
  return 3;
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

  KDCoordinate largeGlyphWidth = KDFont::LargeFont->glyphSize().width();
  KDCoordinate editableZoneWidth = 12 * KDFont::SmallFont->glyphSize().width();
  KDCoordinate editableZoneHeight = KDFont::SmallFont->glyphSize().height();

  switch(step) {
    case Step::FirstParameter:
      m_editableZone.setFrame(KDRect(
        2 * largeGlyphWidth,
        k_symbolHeightMargin + k_sigmaHeight/2,
        editableZoneWidth,
        editableZoneHeight
      ));
      return;
    case Step::SecondParameter:
      m_editableZone.setFrame(KDRect(
        2 * largeGlyphWidth,
        k_symbolHeightMargin + k_sigmaHeight/2 - editableZoneHeight,
        editableZoneWidth,
        editableZoneHeight
      ));
      return;
    default:
      m_editableZone.setFrame(KDRectZero);
  }
}

}
