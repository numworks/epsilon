#include "storage_sum_graph_controller.h"
#include "function_app.h"
#include "../apps_container.h"
#include <poincare_layouts.h>
#include <poincare/layout_helper.h>
#include "poincare_helpers.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Poincare;

namespace Shared {

StorageSumGraphController::StorageSumGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, FunctionGraphView * graphView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, CodePoint sumSymbol) :
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

void StorageSumGraphController::viewWillAppear() {
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


void StorageSumGraphController::didEnterResponderChain(Responder * previousFirstResponder) {
  app()->setFirstResponder(m_legendView.textField());
}

bool StorageSumGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return handleZoom(event);
  }
  if ((int)m_step > 1 && event != Ion::Events::OK && event != Ion::Events::EXE && event != Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Left && !m_legendView.textField()->isEditing()) {
    if ((int)m_step > 0 && m_startSum >= m_cursor->x()) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(cursorNextStep(m_cursor->x(), -1))) {
      m_graphView->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Right && !m_legendView.textField()->isEditing()) {
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

bool StorageSumGraphController::moveCursorHorizontallyToPosition(double x) {
  FunctionApp * myApp = static_cast<FunctionApp *>(app());
  assert(!m_record.isNull());
  ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);
  double y = function->evaluateAtAbscissa(x, myApp->localContext());
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

void StorageSumGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

bool StorageSumGraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
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

bool StorageSumGraphController::textFieldDidAbortEditing(TextField * textField) {
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

bool StorageSumGraphController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    return handleEnter();
  }
  if (m_step == Step::Result) {
    return handleEvent(event);
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

bool StorageSumGraphController::handleEnter() {
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
  FunctionApp * myApp = static_cast<FunctionApp *>(app());
  assert(!m_record.isNull());
  ExpiringPointer<Function> function = myApp->functionStore()->modelForRecord(m_record);
  double sum = function->sumBetweenBounds(m_startSum, m_endSum, myApp->localContext());
  m_legendView.setSumSymbol(m_step, m_startSum, m_endSum, sum, createFunctionLayout(function));
  m_legendView.setLegendMessage(I18n::Message::Default, m_step);
  m_graphView->setAreaHighlightColor(true);
  m_graphView->setCursorView(nullptr);
  myApp->setFirstResponder(this);
  return true;
}

/* Legend View */

StorageSumGraphController::LegendView::LegendView(StorageSumGraphController * controller, InputEventHandlerDelegate * inputEventHandlerDelegate, CodePoint sumSymbol) :
  m_sum(0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumLayout(),
  m_legend(k_font, I18n::Message::Default, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_editableZone(controller, m_draftText, m_draftText, TextField::maxBufferSize(), inputEventHandlerDelegate, controller, false, k_font, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumSymbol(sumSymbol)
{
  m_draftText[0] = 0;
}

void StorageSumGraphController::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::GreyMiddle);
}

KDSize StorageSumGraphController::LegendView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, k_legendHeight);
}

void StorageSumGraphController::LegendView::setLegendMessage(I18n::Message message, Step step) {
  m_legend.setMessage(message);
  layoutSubviews(step);
}

void StorageSumGraphController::LegendView::setEditableZone(double d) {
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits)];
  PrintFloat::convertFloatToText<double>(d, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
 m_editableZone.setText(buffer);
}

void StorageSumGraphController::LegendView::setSumSymbol(Step step, double start, double end, double result, Layout functionLayout) {
  assert(step == Step::Result || functionLayout.isUninitialized());
  constexpr int sigmaLength = 2;
  const CodePoint sigma[sigmaLength] = {UCodePointSpace, m_sumSymbol};
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
    Layout start = LayoutHelper::String(buffer, strlen(buffer), KDFont::SmallFont);
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

int StorageSumGraphController::LegendView::numberOfSubviews() const {
  return 3;
}

View * StorageSumGraphController::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return &m_sum;
  }
  if (index == 1) {
    return &m_editableZone;
  }
  return &m_legend;
}

void StorageSumGraphController::LegendView::layoutSubviews() {
  layoutSubviews(Step::FirstParameter);
}

void StorageSumGraphController::LegendView::layoutSubviews(Step step) {
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

  KDSize largeCharSize = KDFont::LargeFont->glyphSize();
  switch(step) {
    case Step::FirstParameter:
      m_editableZone.setFrame(KDRect(2*largeCharSize.width(), k_symbolHeightMargin+k_sigmaHeight/2, editableZoneWidth(), editableZoneHeight()));
      return;
    case Step::SecondParameter:
      m_editableZone.setFrame(KDRect(2*largeCharSize.width(), k_symbolHeightMargin+k_sigmaHeight/2-editableZoneHeight(), editableZoneWidth(), editableZoneHeight()));
      return;
    default:
      m_editableZone.setFrame(KDRectZero);
  }
}

}
