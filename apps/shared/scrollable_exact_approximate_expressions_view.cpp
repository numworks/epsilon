#include "scrollable_exact_approximate_expressions_view.h"
#include "../i18n.h"
#include <assert.h>
using namespace Poincare;

namespace Shared {

ScrollableExactApproximateExpressionsView::ContentCell::ContentCell() :
  m_approximateExpressionView(),
  m_approximateSign(KDFont::LargeFont, I18n::Message::AlmostEqual, 0.5f, 0.5f, Palette::GreyVeryDark),
  m_exactExpressionView(),
  m_selectedSubviewType((SubviewType)0)
{
}

KDColor ScrollableExactApproximateExpressionsView::ContentCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}

void ScrollableExactApproximateExpressionsView::ContentCell::setHighlighted(bool highlight) {
  // Do not call HighlightCell::setHighlighted to avoid marking all cell as dirty
  m_highlighted = highlight;
  m_exactExpressionView.setBackgroundColor(backgroundColor());
  m_approximateExpressionView.setBackgroundColor(backgroundColor());
  if (highlight) {
    if (m_selectedSubviewType == SubviewType::ExactOutput) {
      m_exactExpressionView.setBackgroundColor(Palette::Select);
    } else {
      m_approximateExpressionView.setBackgroundColor(Palette::Select);
    }
  }
}

void ScrollableExactApproximateExpressionsView::ContentCell::reloadCell() {
  setHighlighted(isHighlighted());
  m_approximateSign.setBackgroundColor(backgroundColor());
  if (numberOfSubviews() == 1) {
    m_approximateExpressionView.setTextColor(KDColorBlack);
  } else {
    m_approximateExpressionView.setTextColor(Palette::GreyVeryDark);
  }
  layoutSubviews();
}

KDSize ScrollableExactApproximateExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  KDSize approximateExpressionSize = m_approximateExpressionView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    return approximateExpressionSize;
  }
  KDSize exactExpressionSize = m_exactExpressionView.minimalSizeForOptimalDisplay();
  KDCoordinate exactBaseline = m_exactExpressionView.layout().baseline();
  KDCoordinate approximateBaseline = m_approximateExpressionView.layout().baseline();
  KDCoordinate height = max(exactBaseline, approximateBaseline) + max(exactExpressionSize.height()-exactBaseline, approximateExpressionSize.height()-approximateBaseline);
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  return KDSize(exactExpressionSize.width()+approximateSignSize.width()+approximateExpressionSize.width()+2*k_digitHorizontalMargin, height);
}

void ScrollableExactApproximateExpressionsView::ContentCell::setSelectedSubviewType(ScrollableExactApproximateExpressionsView::SubviewType subviewType) {
  m_selectedSubviewType = subviewType;
  setHighlighted(isHighlighted());
}

Poincare::Layout ScrollableExactApproximateExpressionsView::ContentCell::layout() const {
  if (m_selectedSubviewType == SubviewType::ExactOutput) {
    return m_exactExpressionView.layout();
  } else {
    return m_approximateExpressionView.layout();
  }
}

int ScrollableExactApproximateExpressionsView::ContentCell::numberOfSubviews() const {
  if (m_exactExpressionView.layout().isUninitialized()) {
    return 1;
  }
  return 3;
}

View * ScrollableExactApproximateExpressionsView::ContentCell::subviewAtIndex(int index) {
  View * views[3] = {&m_approximateExpressionView, &m_approximateSign, &m_exactExpressionView};
  return views[index];
}

void ScrollableExactApproximateExpressionsView::ContentCell::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDSize approximateExpressionSize = m_approximateExpressionView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    m_approximateExpressionView.setFrame(KDRect(0, 0, approximateExpressionSize.width(), height));
    return;
  }
  KDCoordinate exactBaseline = m_exactExpressionView.layout().baseline();
  KDCoordinate approximateBaseline = m_approximateExpressionView.layout().baseline();
  KDCoordinate baseline = max(exactBaseline, approximateBaseline);
  KDSize exactExpressionSize = m_exactExpressionView.minimalSizeForOptimalDisplay();
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  m_exactExpressionView.setFrame(KDRect(0, baseline-exactBaseline, exactExpressionSize));
  m_approximateExpressionView.setFrame(KDRect(2*k_digitHorizontalMargin+exactExpressionSize.width()+approximateSignSize.width(), baseline-approximateBaseline, approximateExpressionSize));
  m_approximateSign.setFrame(KDRect(k_digitHorizontalMargin+exactExpressionSize.width(), baseline-approximateSignSize.height()/2, approximateSignSize));
}

ScrollableExactApproximateExpressionsView::ScrollableExactApproximateExpressionsView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_contentCell, this),
  m_contentCell()
{
}

void ScrollableExactApproximateExpressionsView::setLayouts(Poincare::Layout approximateLayout, Poincare::Layout exactLayout) {
  m_contentCell.approximateExpressionView()->setLayout(approximateLayout);
  m_contentCell.exactExpressionView()->setLayout(exactLayout);
  m_contentCell.layoutSubviews();
}

void ScrollableExactApproximateExpressionsView::setEqualMessage(I18n::Message equalSignMessage) {
  m_contentCell.approximateSign()->setMessage(equalSignMessage);
}

void ScrollableExactApproximateExpressionsView::didBecomeFirstResponder() {
  if (m_contentCell.exactExpressionView()->layout().isUninitialized()) {
    setSelectedSubviewType(SubviewType::ApproximativeOutput);
  } else {
    setSelectedSubviewType(SubviewType::ExactOutput);
  }
}

bool ScrollableExactApproximateExpressionsView::handleEvent(Ion::Events::Event event) {
  if (m_contentCell.exactExpressionView()->layout().isUninitialized()) {
    return ScrollableView::handleEvent(event);
  }
  bool rightExpressionIsVisible = minimalSizeForOptimalDisplay().width() - m_contentCell.approximateExpressionView()->minimalSizeForOptimalDisplay().width() - m_manualScrollingOffset.x() < bounds().width()
;
  bool leftExpressionIsVisible = m_contentCell.exactExpressionView()->minimalSizeForOptimalDisplay().width() - m_manualScrollingOffset.x() > 0;
  if ((event == Ion::Events::Right && selectedSubviewType() == SubviewType::ExactOutput && rightExpressionIsVisible) ||
    (event == Ion::Events::Left && selectedSubviewType() == SubviewType::ApproximativeOutput && leftExpressionIsVisible)) {
    SubviewType otherSubviewType = selectedSubviewType() == SubviewType::ExactOutput ? SubviewType::ApproximativeOutput : SubviewType::ExactOutput;
    setSelectedSubviewType(otherSubviewType);
    return true;
  }
  return ScrollableView::handleEvent(event);
}

KDSize ScrollableExactApproximateExpressionsView::minimalSizeForOptimalDisplay() const {
  return m_contentCell.minimalSizeForOptimalDisplay();
}

}
