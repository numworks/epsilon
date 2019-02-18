#include "scrollable_exact_approximate_expressions_view.h"
#include "../i18n.h"
#include <assert.h>
using namespace Poincare;

namespace Shared {

ScrollableExactApproximateExpressionsView::ContentCell::ContentCell() :
  m_rightExpressionView(),
  m_approximateSign(KDFont::LargeFont, I18n::Message::AlmostEqual, 0.5f, 0.5f, Palette::GreyVeryDark),
  m_leftExpressionView(),
  m_selectedSubviewPosition((SubviewPosition)0)
{
}

KDColor ScrollableExactApproximateExpressionsView::ContentCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}

void ScrollableExactApproximateExpressionsView::ContentCell::setHighlighted(bool highlight) {
  // Do not call HighlightCell::setHighlighted to avoid marking all cell as dirty
  m_highlighted = highlight;
  m_leftExpressionView.setBackgroundColor(backgroundColor());
  m_rightExpressionView.setBackgroundColor(backgroundColor());
  if (highlight) {
    if (m_selectedSubviewPosition == SubviewPosition::Left) {
      m_leftExpressionView.setBackgroundColor(Palette::Select);
    } else {
      m_rightExpressionView.setBackgroundColor(Palette::Select);
    }
  }
}

void ScrollableExactApproximateExpressionsView::ContentCell::reloadCell() {
  setHighlighted(isHighlighted());
  m_approximateSign.setBackgroundColor(backgroundColor());
  if (numberOfSubviews() == 1) {
    m_rightExpressionView.setTextColor(KDColorBlack);
  } else {
    m_rightExpressionView.setTextColor(Palette::GreyVeryDark);
  }
  layoutSubviews();
}

KDSize ScrollableExactApproximateExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    return rightExpressionSize;
  }
  KDSize leftExpressionSize = m_leftExpressionView.minimalSizeForOptimalDisplay();
  KDCoordinate leftBaseline = m_leftExpressionView.layout().baseline();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().baseline();
  KDCoordinate height = max(leftBaseline, rightBaseline) + max(leftExpressionSize.height()-leftBaseline, rightExpressionSize.height()-rightBaseline);
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  return KDSize(leftExpressionSize.width()+approximateSignSize.width()+rightExpressionSize.width()+2*k_digitHorizontalMargin, height);
}

void ScrollableExactApproximateExpressionsView::ContentCell::setSelectedSubviewPosition(ScrollableExactApproximateExpressionsView::SubviewPosition subviewPosition) {
  m_selectedSubviewPosition = subviewPosition;
  setHighlighted(isHighlighted());
}

Poincare::Layout ScrollableExactApproximateExpressionsView::ContentCell::layout() const {
  if (m_selectedSubviewPosition == SubviewPosition::Left) {
    return m_leftExpressionView.layout();
  } else {
    return m_rightExpressionView.layout();
  }
}

int ScrollableExactApproximateExpressionsView::ContentCell::numberOfSubviews() const {
  if (m_leftExpressionView.layout().isUninitialized()) {
    return 1;
  }
  return 3;
}

View * ScrollableExactApproximateExpressionsView::ContentCell::subviewAtIndex(int index) {
  View * views[3] = {&m_rightExpressionView, &m_approximateSign, &m_leftExpressionView};
  return views[index];
}

void ScrollableExactApproximateExpressionsView::ContentCell::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    m_rightExpressionView.setFrame(KDRect(0, 0, rightExpressionSize.width(), height));
    return;
  }
  KDCoordinate leftBaseline = m_leftExpressionView.layout().baseline();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().baseline();
  KDCoordinate baseline = max(leftBaseline, rightBaseline);
  KDSize leftExpressionSize = m_leftExpressionView.minimalSizeForOptimalDisplay();
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  m_leftExpressionView.setFrame(KDRect(0, baseline-leftBaseline, leftExpressionSize));
  m_rightExpressionView.setFrame(KDRect(2*k_digitHorizontalMargin+leftExpressionSize.width()+approximateSignSize.width(), baseline-rightBaseline, rightExpressionSize));
  m_approximateSign.setFrame(KDRect(k_digitHorizontalMargin+leftExpressionSize.width(), baseline-approximateSignSize.height()/2, approximateSignSize));
}

ScrollableExactApproximateExpressionsView::ScrollableExactApproximateExpressionsView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_contentCell, this),
  m_contentCell()
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
  setMargins(0, Metric::HistoryHorizontalMargin, 0, Metric::HistoryHorizontalMargin);
}

void ScrollableExactApproximateExpressionsView::setLayouts(Poincare::Layout rightLayout, Poincare::Layout leftLayout) {
  m_contentCell.rightExpressionView()->setLayout(rightLayout);
  m_contentCell.leftExpressionView()->setLayout(leftLayout);
  m_contentCell.layoutSubviews();
}

void ScrollableExactApproximateExpressionsView::setEqualMessage(I18n::Message equalSignMessage) {
  m_contentCell.approximateSign()->setMessage(equalSignMessage);
}

void ScrollableExactApproximateExpressionsView::didBecomeFirstResponder() {
  if (m_contentCell.leftExpressionView()->layout().isUninitialized()) {
    setSelectedSubviewPosition(SubviewPosition::Right);
  } else {
    setSelectedSubviewPosition(SubviewPosition::Left);
  }
}

bool ScrollableExactApproximateExpressionsView::handleEvent(Ion::Events::Event event) {
  if (m_contentCell.leftExpressionView()->layout().isUninitialized()) {
    return ScrollableView::handleEvent(event);
  }
  bool rightExpressionIsVisible = minimalSizeForOptimalDisplay().width() - m_contentCell.rightExpressionView()->minimalSizeForOptimalDisplay().width() - m_manualScrollingOffset.x() < bounds().width()
;
  bool leftExpressionIsVisible = m_contentCell.leftExpressionView()->minimalSizeForOptimalDisplay().width() - m_manualScrollingOffset.x() > 0;
  if ((event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Left && rightExpressionIsVisible) ||
    (event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && leftExpressionIsVisible)) {
    SubviewPosition otherSubviewPosition = selectedSubviewPosition() == SubviewPosition::Left ? SubviewPosition::Right : SubviewPosition::Left;
    setSelectedSubviewPosition(otherSubviewPosition);
    return true;
  }
  return ScrollableView::handleEvent(event);
}

}
