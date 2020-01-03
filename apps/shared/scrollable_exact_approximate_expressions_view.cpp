#include "scrollable_exact_approximate_expressions_view.h"
#include <apps/i18n.h>
#include <assert.h>
using namespace Poincare;

namespace Shared {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

ScrollableExactApproximateExpressionsView::ContentCell::ContentCell() :
  m_rightExpressionView(),
  m_approximateSign(KDFont::LargeFont, I18n::Message::AlmostEqual, 0.5f, 0.5f, Palette::GreyVeryDark),
  m_leftExpressionView(),
  m_selectedSubviewPosition((SubviewPosition)0),
  m_displayLeftExpression(true)
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
  m_approximateSign.setBackgroundColor(backgroundColor());
  if (highlight) {
    if (m_selectedSubviewPosition == SubviewPosition::Left) {
      m_leftExpressionView.setBackgroundColor(Palette::Select);
    } else {
      m_rightExpressionView.setBackgroundColor(Palette::Select);
    }
  }
}

void ScrollableExactApproximateExpressionsView::ContentCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_leftExpressionView.setBackgroundColor(backgroundColor());
  m_rightExpressionView.setBackgroundColor(backgroundColor());
  m_approximateSign.setBackgroundColor(backgroundColor());
}

void ScrollableExactApproximateExpressionsView::ContentCell::reloadTextColor() {
  if (numberOfSubviews() == 1) {
    m_rightExpressionView.setTextColor(KDColorBlack);
  } else {
    m_rightExpressionView.setTextColor(Palette::GreyVeryDark);
  }
}

KDSize ScrollableExactApproximateExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    return rightExpressionSize;
  }
  KDSize leftExpressionSize = m_leftExpressionView.minimalSizeForOptimalDisplay();
  KDCoordinate leftBaseline = m_leftExpressionView.layout().baseline();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().baseline();
  KDCoordinate height = maxCoordinate(leftBaseline, rightBaseline) + maxCoordinate(leftExpressionSize.height()-leftBaseline, rightExpressionSize.height()-rightBaseline);
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  return KDSize(leftExpressionSize.width()+approximateSignSize.width()+rightExpressionSize.width()+2*Metric::CommonLargeMargin, height);
}

void ScrollableExactApproximateExpressionsView::ContentCell::setSelectedSubviewPosition(ScrollableExactApproximateExpressionsView::SubviewPosition subviewPosition) {
  m_selectedSubviewPosition = subviewPosition;
  setHighlighted(isHighlighted());
}

void ScrollableExactApproximateExpressionsView::ContentCell::setDisplayLeftExpression(bool display) {
  m_displayLeftExpression = display;
  reloadTextColor();
}

Poincare::Layout ScrollableExactApproximateExpressionsView::ContentCell::layout() const {
  if (m_selectedSubviewPosition == SubviewPosition::Left) {
    return m_leftExpressionView.layout();
  } else {
    return m_rightExpressionView.layout();
  }
}

int ScrollableExactApproximateExpressionsView::ContentCell::numberOfSubviews() const {
  if (!m_displayLeftExpression || m_leftExpressionView.layout().isUninitialized()) {
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
  KDCoordinate baseline = maxCoordinate(leftBaseline, rightBaseline);
  KDSize leftExpressionSize = m_leftExpressionView.minimalSizeForOptimalDisplay();
  KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
  m_leftExpressionView.setFrame(KDRect(0, baseline-leftBaseline, leftExpressionSize));
  m_rightExpressionView.setFrame(KDRect(2*Metric::CommonLargeMargin+leftExpressionSize.width()+approximateSignSize.width(), baseline-rightBaseline, rightExpressionSize));
  m_approximateSign.setFrame(KDRect(Metric::CommonLargeMargin+leftExpressionSize.width(), baseline-approximateSignSize.height()/2, approximateSignSize));
}

ScrollableExactApproximateExpressionsView::ScrollableExactApproximateExpressionsView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_contentCell, this),
  m_contentCell()
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
  setMargins(
    Metric::CommonSmallMargin,
    Metric::CommonLargeMargin,
    Metric::CommonSmallMargin,
    Metric::CommonLargeMargin
  );
}

void ScrollableExactApproximateExpressionsView::setLayouts(Poincare::Layout rightLayout, Poincare::Layout leftLayout) {
  bool updateRightLayout = m_contentCell.rightExpressionView()->setLayout(rightLayout);
  bool updateLeftLayout = m_contentCell.leftExpressionView()->setLayout(leftLayout);
  if (updateRightLayout || updateLeftLayout) {
    m_contentCell.reloadTextColor();
    m_contentCell.layoutSubviews();
    reloadScroll();
  }
}

void ScrollableExactApproximateExpressionsView::setEqualMessage(I18n::Message equalSignMessage) {
  m_contentCell.approximateSign()->setMessage(equalSignMessage);
}

void ScrollableExactApproximateExpressionsView::reloadScroll() {
  if (selectedSubviewPosition() == SubviewPosition::Left) {
    // Scroll to the left extremity
    ScrollableView::reloadScroll();
  } else {
    // Scroll to the right extremity
    scrollToContentPoint(KDPoint(m_contentCell.bounds().width(), 0), true);
  }
}

void ScrollableExactApproximateExpressionsView::didBecomeFirstResponder() {
  if (m_contentCell.leftExpressionView()->layout().isUninitialized()) {
    setSelectedSubviewPosition(SubviewPosition::Right);
  }
  if (m_contentCell.rightExpressionView()->layout().isUninitialized()) {
    setSelectedSubviewPosition(SubviewPosition::Left);
  }
}

bool ScrollableExactApproximateExpressionsView::handleEvent(Ion::Events::Event event) {
  if (!m_contentCell.displayLeftExpression()) {
    return ScrollableView::handleEvent(event);
  }
  bool rightExpressionIsVisible = minimalSizeForOptimalDisplay().width() - m_contentCell.rightExpressionView()->minimalSizeForOptimalDisplay().width() - contentOffset().x() < bounds().width()
;
  bool leftExpressionIsVisible = m_contentCell.leftExpressionView()->minimalSizeForOptimalDisplay().width() - contentOffset().x() > 0;
  if ((event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Left && rightExpressionIsVisible) ||
    (event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && leftExpressionIsVisible)) {
    SubviewPosition otherSubviewPosition = selectedSubviewPosition() == SubviewPosition::Left ? SubviewPosition::Right : SubviewPosition::Left;
    setSelectedSubviewPosition(otherSubviewPosition);
    return true;
  }
  return ScrollableView::handleEvent(event);
}

}
