#include "term_sum_controller.h"
#include "../../shared/text_field_delegate.h"
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"
#include "../../../poincare/src/layout/horizontal_layout.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

TermSumController::TermSumController(Responder * parentResponder, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor) :
  ViewController(parentResponder),
  m_contentView(ContentView(graphView)),
  m_graphRange(graphRange),
  m_sequence(nullptr),
  m_cursor(cursor),
  m_cursorView(VerticalCursorView()),
  m_step(0),
  m_startSum(-1),
  m_endSum(-1)
{
}

const char * TermSumController::title() const {
  return "Sommes des termes";
}

View * TermSumController::view() {
  return &m_contentView;
}

void TermSumController::viewWillAppear() {
  m_contentView.graphView()->setVerticalCursor(true);
  m_contentView.graphView()->setCursorView(&m_cursorView);
  m_contentView.graphView()->setBannerView(nullptr);
  m_contentView.graphView()->setOkView(nullptr);
  m_contentView.graphView()->selectMainView(true);
  m_contentView.graphView()->setHighlightColor(false);
  m_contentView.graphView()->setHighlight(-1.0f,-1.0f);
  m_contentView.graphView()->reload();
  m_contentView.layoutSubviews();

  m_startSum = -1;
  m_endSum = -1;
  m_step = 0;
  m_contentView.legendView()->setLegendText("SELECTIONNER LE PREMIER TERME");
  m_contentView.legendView()->setSumSubscript(m_cursor->x());
}

bool TermSumController::handleEvent(Ion::Events::Event event) {
  if (m_step > 1 && event != Ion::Events::OK) {
    return false;
  }
  if (event == Ion::Events::Left) {
    if (m_step > 0 && m_startSum >= m_cursor->x()) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(roundf(m_cursor->x()-1.0f))) {
      m_contentView.graphView()->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Right) {
    if (moveCursorHorizontallyToPosition(roundf(m_cursor->x()+1.0f))) {
      m_contentView.graphView()->reload();
      return true;
    }
    return false;
  }
  if (event.hasText() && event.text()[0] >= '0' && event.text()[0] <= '9') {
    if (m_step > 0 && event.text()[0]-'0' < m_cursor->x()) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(event.text()[0]-'0')) {
      m_contentView.graphView()->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::OK) {
    if (m_step == 2) {
      StackViewController * stack = (StackViewController *)parentResponder();
      stack->pop();
      return true;
    }
    if (m_step == 0) {
      m_step++;
      m_startSum = m_cursor->x();
      m_contentView.legendView()->setSumSuperscript(m_startSum, m_cursor->x());
      m_contentView.legendView()->setLegendText("SELECTIONNER LE DERNIER TERME");
      return true;
    }
    m_step++;
    m_endSum = m_cursor->x();
    m_contentView.legendView()->setSequenceName(m_sequence->name());
    char buffer[2+Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    strlcpy(buffer, "= ", 3);
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    float sum = m_sequence->sumOfTermsBetweenAbscissa(m_startSum, m_endSum, myApp->localContext());
    Complex::convertFloatToText(sum, buffer+2, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    m_contentView.graphView()->setHighlightColor(true);
    m_contentView.graphView()->selectMainView(false);
    m_contentView.legendView()->setLegendText(buffer);
  }
  return false;
}

bool TermSumController::moveCursorHorizontallyToPosition(int position) {
  if (position < 0) {
    return false;
  }
  float x = position;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  if (m_sequence == nullptr) {
    return false;
  }
  float y = m_sequence->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  if (m_step == 0) {
    m_contentView.legendView()->setSumSubscript(m_cursor->x());
  }
  if (m_step == 1) {
    m_contentView.graphView()->setHighlight(m_startSum, m_cursor->x());
    m_contentView.legendView()->setSumSuperscript(m_startSum, m_cursor->x());
  }
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

void TermSumController::setSequence(Sequence * sequence) {
  m_contentView.graphView()->selectSequence(sequence);
  m_sequence = sequence;
}

/* Content View */

TermSumController::ContentView::ContentView(GraphView * graphView) :
  m_graphView(graphView)
{
}

void TermSumController::ContentView::layoutSubviews() {
  m_graphView->setFrame(bounds());
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight));
}

GraphView * TermSumController::ContentView::graphView() {
  return m_graphView;
}

TermSumController::ContentView::LegendView * TermSumController::ContentView::legendView() {
  return &m_legendView;
}

int TermSumController::ContentView::numberOfSubviews() const {
  return 2;
}

View * TermSumController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_graphView;
  }
  return &m_legendView;
}

/* Legend View */

TermSumController::ContentView::LegendView::LegendView() :
  m_sum(ExpressionView(0.0f, 0.5f, KDColorBlack, Palette::GreyBright)),
  m_sumLayout(nullptr),
  m_legend(BufferTextView(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyBright))
{
}

TermSumController::ContentView::LegendView::~LegendView() {
  if (m_sumLayout != nullptr) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
}

void TermSumController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::GreyBright);
}

void TermSumController::ContentView::LegendView::setLegendText(const char * text) {
  m_legend.setText(text);
  layoutSubviews();
}

void TermSumController::ContentView::LegendView::setSumSubscript(float start) {
  if (m_sumLayout) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
  const char sigma[2] = {Ion::Charset::CapitalSigma, 0};
  char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex::convertFloatToText(start, buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  m_sumLayout = new BaselineRelativeLayout(new StringLayout(sigma, 1), new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
 m_sum.setExpression(m_sumLayout);
 m_sum.setAlignment(0.0f, 0.5f);
 layoutSubviews();
}

void TermSumController::ContentView::LegendView::setSumSuperscript(float start, float end) {
  if (m_sumLayout) {
    delete m_sumLayout;
    m_sumLayout = nullptr;
  }
  const char sigma[2] = {Ion::Charset::CapitalSigma, 0};
  char bufferStart[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex::convertFloatToText(start, bufferStart, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  char bufferEnd[Complex::bufferSizeForFloatsWithPrecision(1)];
  Complex::convertFloatToText(end, bufferEnd, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  ExpressionLayout * sigmaLayout = new BaselineRelativeLayout(new StringLayout(sigma, 1), new StringLayout(bufferStart, strlen(bufferStart), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  m_sumLayout = new BaselineRelativeLayout(sigmaLayout, new StringLayout(bufferEnd, strlen(bufferEnd), KDText::FontSize::Small), BaselineRelativeLayout::Type::Superscript);
  m_sum.setExpression(m_sumLayout);
  layoutSubviews();
}

void TermSumController::ContentView::LegendView::setSequenceName(const char * sequenceName) {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  childrenLayouts[1] = new BaselineRelativeLayout(new StringLayout(sequenceName, 1, KDText::FontSize::Small), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[0] = m_sumLayout;
  m_sumLayout = new HorizontalLayout(childrenLayouts, 2);
  m_sum.setExpression(m_sumLayout);
  m_sum.setAlignment(1.0f, 0.5f);
  layoutSubviews();
}

int TermSumController::ContentView::LegendView::numberOfSubviews() const {
  return 2;
}

View * TermSumController::ContentView::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_sum;
  }
  return &m_legend;
}

void TermSumController::ContentView::LegendView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate heigth = bounds().height();
  KDSize legendSize = m_legend.minimalSizeForOptimalDisplay();
  if (legendSize.width() > width/2) {
    m_sum.setFrame(KDRect(0, 0, width-legendSize.width(), heigth));
    m_legend.setFrame(KDRect(width-legendSize.width(), 0, legendSize.width(), heigth));
    return;
  }
  m_sum.setFrame(KDRect(0, 0, width/2, heigth));
  m_legend.setFrame(KDRect(width/2, 0, width/2, heigth));
}

}
