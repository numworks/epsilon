#include "output_expressions_view.h"
#include "../i18n.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

OutputExpressionsView::OutputExpressionsView(Responder * parentResponder) :
  Responder(parentResponder),
  m_approximateExpressionView(),
  m_approximateSign(KDText::FontSize::Large, I18n::Message::ApproximateEqual, 0.5f, 0.5f),
  m_exactExpressionView(),
  m_selectedSubviewType(OutputExpressionsView::SubviewType::ExactOutput)
{
}

void OutputExpressionsView::setExpressions(ExpressionLayout ** expressionsLayout) {
  m_approximateExpressionView.setExpression(expressionsLayout[0]);
  m_exactExpressionView.setExpression(expressionsLayout[1]);
  layoutSubviews();
}

KDColor OutputExpressionsView::backgroundColor() const {
  KDColor background = m_even ? Palette::WallScreen : KDColorWhite;
  return background;
}

void OutputExpressionsView::reloadCell() {
  m_exactExpressionView.setBackgroundColor(backgroundColor());
  m_approximateSign.setBackgroundColor(backgroundColor());
  m_approximateExpressionView.setBackgroundColor(backgroundColor());
  if (isHighlighted()) {
    if (m_selectedSubviewType == SubviewType::ExactOutput) {
      m_exactExpressionView.setBackgroundColor(Palette::Select);
    } else {
      m_approximateExpressionView.setBackgroundColor(Palette::Select);
    }
  }
  layoutSubviews();
  EvenOddCell::reloadCell();
}

KDSize OutputExpressionsView::minimalSizeForOptimalDisplay() const {
  KDSize approximateExpressionSize = m_approximateExpressionView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    return KDSize(approximateExpressionSize.width() + 2*k_digitHorizontalMargin, approximateExpressionSize.height());
  }
  KDSize exactExpressionSize = m_exactExpressionView.minimalSizeForOptimalDisplay();
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  return KDSize(exactExpressionSize.width()+approximateSignSize.width()+approximateExpressionSize.width()+4*k_digitHorizontalMargin, exactExpressionSize.height() > approximateExpressionSize.height() ? exactExpressionSize.height() : approximateExpressionSize.height());
}

void OutputExpressionsView::didBecomeFirstResponder() {
  if (numberOfSubviews() == 1) {
    setSelectedSubviewType(SubviewType::ApproximativeOutput);
  } else {
    setSelectedSubviewType(SubviewType::ExactOutput);
  }
}

bool OutputExpressionsView::handleEvent(Ion::Events::Event event) {
  if (numberOfSubviews() == 1) {
    return false;
  }
  if ((event == Ion::Events::Left && m_selectedSubviewType == SubviewType::ExactOutput) ||
    (event == Ion::Events::Right && m_selectedSubviewType == SubviewType::ApproximativeOutput)) {
    SubviewType otherSubviewType = m_selectedSubviewType == SubviewType::ExactOutput ? SubviewType::ApproximativeOutput : SubviewType::ExactOutput;
    setSelectedSubviewType(otherSubviewType);
    reloadCell();
    // TODO: something with scrolling parent() == Scrolling !
    return true;
  }
  return false;
}

OutputExpressionsView::SubviewType OutputExpressionsView::selectedSubviewType() {
  return m_selectedSubviewType;
}

void OutputExpressionsView::setSelectedSubviewType(OutputExpressionsView::SubviewType subviewType) {
  m_selectedSubviewType = subviewType;
}

int OutputExpressionsView::numberOfSubviews() const {
  if (m_exactExpressionView.expressionLayout() != nullptr) {
    return 3;
  }
  return 1;
}

View * OutputExpressionsView::subviewAtIndex(int index) {
  View * views[3] = {&m_approximateExpressionView, &m_approximateSign, &m_exactExpressionView};
  return views[index];
}

void OutputExpressionsView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDSize approximateExpressionSize = m_approximateExpressionView.minimalSizeForOptimalDisplay();
  m_approximateExpressionView.setFrame(KDRect(k_digitHorizontalMargin, 0, approximateExpressionSize.width(), height));
  if (numberOfSubviews() == 1) {
    return;
  }
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  KDSize exactExpressionSize = m_exactExpressionView.minimalSizeForOptimalDisplay();
  m_approximateSign.setFrame(KDRect(2*k_digitHorizontalMargin+approximateExpressionSize.width(), 0, approximateSignSize.width(), height));
  m_exactExpressionView.setFrame(KDRect(3*k_digitHorizontalMargin+approximateExpressionSize.width()+approximateSignSize.width(), 0, exactExpressionSize.width(), height));
}

}
