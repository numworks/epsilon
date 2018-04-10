#include "term_sum_controller.h"
#include "../../shared/text_field_delegate.h"
#include "../app.h"

#include <poincare/layout_engine.h>
#include "../../../poincare/src/layout/char_layout.h"
#include "../../../poincare/src/layout/condensed_sum_layout.h"
#include "../../../poincare/src/layout/horizontal_layout.h"
#include "../../../poincare/src/layout/vertical_offset_layout.h"

#include <cmath>

extern "C" {
#include <assert.h>
#include <stdlib.h>
}

using namespace Shared;
using namespace Poincare;

namespace Sequence {

TermSumController::TermSumController(Responder * parentResponder, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor) :
  SumGraphController(parentResponder, graphView, graphRange, cursor, Ion::Charset::CapitalSigma)
{
}

const char * TermSumController::title() {
  return I18n::translate(I18n::Message::TermSum);
}

bool TermSumController::moveCursorHorizontallyToPosition(double position) {
  if (position < 0.0) {
    return false;
  }
  return SumGraphController::moveCursorHorizontallyToPosition(std::round(position));
}

I18n::Message TermSumController::legendMessageAtStep(Step step) {
  switch(step) {
    case Step::FirstParameter:
      return I18n::Message::SelectFirstTerm;
    case Step::SecondParameter:
      return I18n::Message::SelectLastTerm;
    default:
      return I18n::Message::Default;
  }
}

double TermSumController::cursorNextStep(double x, int direction) {
  double delta = direction > 0 ? 1.0 : -1.0;
  return std::round(m_cursor->x()+delta);
}

<<<<<<< HEAD
ExpressionLayout * TermSumController::createFunctionLayout(const char * functionName) {
  return new BaselineRelativeLayout(new StringLayout(functionName, 1, KDText::FontSize::Small), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
=======
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
  const char sigma[] = {' ', Ion::Charset::CapitalSigma};
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex<float>::convertFloatToText(start, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  m_sumLayout = new CondensedSumLayout(
      LayoutEngine::createStringLayout(sigma, sizeof(sigma), KDText::FontSize::Large),
      LayoutEngine::createStringLayout(buffer, strlen(buffer), KDText::FontSize::Small),
      nullptr,
      false);
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
  m_sumLayout = new CondensedSumLayout(
      LayoutEngine::createStringLayout(sigma, sizeof(sigma), KDText::FontSize::Large),
      LayoutEngine::createStringLayout(bufferStart, strlen(bufferStart), KDText::FontSize::Small),
      LayoutEngine::createStringLayout(bufferEnd, strlen(bufferEnd), KDText::FontSize::Small),
      false);
  m_sum.setExpressionLayout(m_sumLayout);
  m_sum.setAlignment(0.0f, 0.5f);
}

void TermSumController::LegendView::setSumResult(const char * sequenceName, double result) {
  char buffer[2+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  strlcpy(buffer, "= ", 3);
  Complex<double>::convertFloatToText(result, buffer+2, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_sumLayout = new HorizontalLayout(
      m_sumLayout,
      new HorizontalLayout(
        new CharLayout(sequenceName[0], KDText::FontSize::Small),
        new VerticalOffsetLayout(new CharLayout('n', KDText::FontSize::Small), VerticalOffsetLayout::Type::Subscript, false),
        false),
      LayoutEngine::createStringLayout(buffer, strlen(buffer), KDText::FontSize::Small),
      false);
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
>>>>>>> SaisieJolieRebase1201
}

}
