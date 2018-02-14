#include "sum_graph_controller.h"
#include "../apps_container.h"
#include "../../poincare/src/layout/condensed_sum_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include "../../poincare/src/layout/horizontal_layout.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Poincare;

namespace Shared {

SumGraphController::SumGraphController(Responder * parentResponder, FunctionGraphView * graphView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, char sumSymbol) :
  SimpleInteractiveCurveViewController(parentResponder, range, graphView, cursor),
  m_step(Step::FirstParameter),
  m_startSum(NAN),
  m_endSum(NAN),
  m_function(nullptr),
  m_graphRange(range),
  m_graphView(graphView),
  m_legendView(this, sumSymbol),
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
  double floatBody = Expression::approximateToScalar<double>(text, *globalContext);
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

bool SumGraphController::textFieldDidAbortEditing(TextField * textField, const char * text) {
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
  PrintFloat::convertFloatToText<double>(parameter, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
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

SumGraphController::LegendView::LegendView(SumGraphController * controller, char sumSymbol) :
  m_sum(0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumLayout(nullptr),
  m_legend(KDText::FontSize::Small, I18n::Message::Default, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_editableZone(controller, m_draftText, m_draftText, TextField::maxBufferSize(), controller, false, KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sumSymbol(sumSymbol)
{
  m_draftText[0] = 0;
}

SumGraphController::LegendView::~LegendView() {
  if (m_sumLayout != nullptr) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
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
  PrintFloat::convertFloatToText<double>(d, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
 m_editableZone.setText(buffer);
}

void SumGraphController::LegendView::setSumSymbol(Step step, double start, double end, double result, ExpressionLayout * functionLayout) {
  assert(step == Step::Result || functionLayout == nullptr);
  if (m_sumLayout) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
  const char sigma[] = {' ', m_sumSymbol};
  if (step == Step::FirstParameter) {
    m_sumLayout = new StringLayout(sigma, sizeof(sigma));
  } else if (step == Step::SecondParameter) {
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
    m_sumLayout = new CondensedSumLayout(new StringLayout(sigma, sizeof(sigma)), new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small), nullptr);
  } else {
    char buffer[2+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
    ExpressionLayout * start =  new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small);
    PrintFloat::convertFloatToText<double>(end, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
    ExpressionLayout * end =  new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small);
    m_sumLayout = new CondensedSumLayout(new StringLayout(sigma, sizeof(sigma)), start, end);

    ExpressionLayout * childrenLayouts[3];
    strlcpy(buffer, "= ", 3);
    PrintFloat::convertFloatToText<double>(result, buffer+2, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    childrenLayouts[2] = new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small);
    childrenLayouts[1] = functionLayout;
    childrenLayouts[0] = m_sumLayout;
    m_sumLayout = new HorizontalLayout(childrenLayouts, 3);
  }
  m_sum.setExpression(m_sumLayout);
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

  KDSize largeCharSize = KDText::charSize();
  switch(step) {
    case Step::FirstParameter:
      m_editableZone.setFrame(KDRect(2*largeCharSize.width(), k_symbolHeightMargin+k_sigmaHeight/2, k_editableZoneWidth, k_editableZoneHeight));
      return;
    case Step::SecondParameter:
      m_editableZone.setFrame(KDRect(2*largeCharSize.width(), k_symbolHeightMargin+k_sigmaHeight/2-k_editableZoneHeight, k_editableZoneWidth, k_editableZoneHeight));
      return;
    default:
      m_editableZone.setFrame(KDRectZero);
  }
}

}
