#include "term_sum_controller.h"
#include "../../shared/text_field_delegate.h"
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/condensed_sum_layout.h"
#include "../../../poincare/src/layout/string_layout.h"
#include "../../../poincare/src/layout/horizontal_layout.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

TermSumController::TermSumController(Responder * parentResponder, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor) :
  ViewController(parentResponder),
  m_graphView(graphView),
  m_legendView(),
  m_graphRange(graphRange),
  m_sequence(nullptr),
  m_cursor(cursor),
  m_cursorView(),
  m_bufferCursorPosition(0),
  m_step(0),
  m_startSum(-1),
  m_endSum(-1)
{
}

const char * TermSumController::title() {
  return I18n::translate(I18n::Message::TermSum);
}

View * TermSumController::view() {
  return m_graphView;
}

void TermSumController::viewWillAppear() {
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  m_graphView->setVerticalCursor(true);
  m_graphView->setBannerView(&m_legendView);
  m_graphView->setCursorView(&m_cursorView);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  m_graphView->setHighlightColor(false);
  m_graphView->setHighlight(-1.0f,-1.0f);
  m_graphView->reload();

  m_bufferCursorPosition = 0;
  m_startSum = -1;
  m_endSum = -1;
  m_step = 0;
  m_legendView.setLegendMessage(I18n::Message::SelectFirstTerm);
  m_legendView.setSumSubscript(m_cursor->x());
}

bool TermSumController::handleEvent(Ion::Events::Event event) {
  if (m_step > 1 && event != Ion::Events::OK && event != Ion::Events::EXE && event != Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Left) {
    if (m_step > 0 && m_startSum >= m_cursor->x()) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(std::round(m_cursor->x()-1.0))) {
      m_graphView->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Right) {
    if (moveCursorHorizontallyToPosition(std::round(m_cursor->x()+1.0))) {
      m_graphView->reload();
      return true;
    }
    return false;
  }
  if (event.hasText() && event.text()[0] >= '0' && event.text()[0] <= '9') {
    m_bufferCursorPosition = 10*m_bufferCursorPosition + event.text()[0]-'0';
    if (m_step > 0 && m_bufferCursorPosition < m_startSum) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(m_bufferCursorPosition)) {
      m_graphView->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (m_step == 2) {
      StackViewController * stack = (StackViewController *)parentResponder();
      stack->pop();
      return true;
    }
    if (m_step == 0) {
      m_step++;
      m_bufferCursorPosition = 0;
      m_startSum = m_cursor->x();
      m_graphView->setHighlight(m_startSum,m_startSum);
      m_legendView.setSumSuperscript(m_startSum, m_cursor->x());
      m_legendView.setLegendMessage(I18n::Message::SelectLastTerm);
      return true;
    }
    m_step++;
    m_endSum = m_cursor->x();
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    double sum = m_sequence->sumOfTermsBetweenAbscissa(m_startSum, m_endSum, myApp->localContext());
    m_legendView.setSumResult(m_sequence->name(), sum);
    m_legendView.setLegendMessage(I18n::Message::Default);
    m_graphView->setHighlightColor(true);
    m_graphView->setCursorView(nullptr);
    m_graphView->reload();
    return true;
  }
  if (event == Ion::Events::Back && m_step > 0) {
    m_step--;
    m_bufferCursorPosition = 0;
    if (m_step == 1) {
      m_legendView.setLegendMessage(I18n::Message::SelectLastTerm);
      m_graphView->setHighlightColor(false);
      m_graphView->setCursorView(&m_cursorView);
      m_graphView->reload();
      m_legendView.setSumSuperscript(m_startSum, m_cursor->x());
    }
    if (m_step == 0) {
      m_graphView->setHighlight(-1,-1);
      moveCursorHorizontallyToPosition(m_startSum);
      m_legendView.setLegendMessage(I18n::Message::SelectFirstTerm);
      m_legendView.setSumSubscript(m_startSum);
      m_graphView->reload();
    }
    return true;
  }
  return false;
}

bool TermSumController::moveCursorHorizontallyToPosition(int position) {
  if (position < 0) {
    return false;
  }
  double x = position;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  if (m_sequence == nullptr) {
    return false;
  }
  double y = m_sequence->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  if (m_step == 0) {
    m_legendView.setSumSubscript(m_cursor->x());
  }
  if (m_step == 1) {
    m_graphView->setHighlight(m_startSum, m_cursor->x());
    m_legendView.setSumSuperscript(m_startSum, m_cursor->x());
  }
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

void TermSumController::setSequence(Sequence * sequence) {
  m_graphView->selectSequence(sequence);
  m_sequence = sequence;
}

/* Legend View */

TermSumController::LegendView::LegendView() :
  m_sum(0.0f, 0.5f, KDColorBlack, Palette::GreyBright),
  m_sumLayout(nullptr),
  m_legend(KDText::FontSize::Small, I18n::Message::Default, 0.0f, 0.5f, KDColorBlack, Palette::GreyBright)
{
}

TermSumController::LegendView::~LegendView() {
  if (m_sumLayout != nullptr) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
}

void TermSumController::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::GreyMiddle);
}

KDSize TermSumController::LegendView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, k_legendHeight);
}

void TermSumController::LegendView::setLegendMessage(I18n::Message message) {
  m_legend.setMessage(message);
  layoutSubviews();
}

void TermSumController::LegendView::setSumSubscript(float start) {
  if (m_sumLayout) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
  const char sigma[] = {' ',Ion::Charset::CapitalSigma};
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex<float>::convertFloatToText(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  m_sumLayout = new CondensedSumLayout(new StringLayout(sigma, sizeof(sigma)), new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small), nullptr);
 m_sum.setExpressionLayout(m_sumLayout);
 m_sum.setAlignment(0.0f, 0.5f);
}

void TermSumController::LegendView::setSumSuperscript(float start, float end) {
  if (m_sumLayout) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
  const char sigma[] = {' ', Ion::Charset::CapitalSigma};
  char bufferStart[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex<float>::convertFloatToText(start, bufferStart, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  char bufferEnd[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex<float>::convertFloatToText(end, bufferEnd, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  m_sumLayout = new CondensedSumLayout(new StringLayout(sigma, sizeof(sigma)), new StringLayout(bufferStart, strlen(bufferStart), KDText::FontSize::Small), new StringLayout(bufferEnd, strlen(bufferEnd), KDText::FontSize::Small));
  m_sum.setExpressionLayout(m_sumLayout);
  m_sum.setAlignment(0.0f, 0.5f);
}

void TermSumController::LegendView::setSumResult(const char * sequenceName, double result) {
  ExpressionLayout * childrenLayouts[3];
  char buffer[2+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  strlcpy(buffer, "= ", 3);
  Complex<double>::convertFloatToText(result, buffer+2, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  childrenLayouts[2] = new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small);
  childrenLayouts[1] = new BaselineRelativeLayout(new StringLayout(sequenceName, 1, KDText::FontSize::Small), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[0] = m_sumLayout;
  m_sumLayout = new HorizontalLayout(childrenLayouts, 3);
  m_sum.setExpressionLayout(m_sumLayout);
  m_sum.setAlignment(0.5f, 0.5f);
}

int TermSumController::LegendView::numberOfSubviews() const {
  return 2;
}

View * TermSumController::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_sum;
  }
  return &m_legend;
}

void TermSumController::LegendView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate heigth = bounds().height();
  KDSize legendSize = m_legend.minimalSizeForOptimalDisplay();
  if (legendSize.width() > 0) {
    m_sum.setFrame(KDRect(0, 0, width-legendSize.width(), heigth));
    m_legend.setFrame(KDRect(width-legendSize.width(), 0, legendSize.width(), heigth));
    return;
  }
  m_sum.setFrame(bounds());
  m_legend.setFrame(KDRectZero);
}

}
