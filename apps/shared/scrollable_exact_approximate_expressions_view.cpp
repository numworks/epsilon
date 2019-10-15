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
  m_selectedSubviewPosition(SubviewPosition::Left),
  m_displayLeftExpression(true),
  m_displayBurger(false)
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
  if (m_displayBurger) {
    burgerMenuView()->setBackgroundColor(backgroundColor());
  }
  if (highlight) {
    if (m_selectedSubviewPosition == SubviewPosition::Left) {
      m_leftExpressionView.setBackgroundColor(Palette::Select);
    } else if (m_selectedSubviewPosition == SubviewPosition::Right) {
      m_rightExpressionView.setBackgroundColor(Palette::Select);
    } else {
      burgerMenuView()->setBackgroundColor(Palette::Select);
    }
  }
}

void ScrollableExactApproximateExpressionsView::ContentCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_leftExpressionView.setBackgroundColor(backgroundColor());
  m_rightExpressionView.setBackgroundColor(backgroundColor());
  m_approximateSign.setBackgroundColor(backgroundColor());
  if (m_displayBurger) {
    burgerMenuView()->setBackgroundColor(backgroundColor());
  }
}

void ScrollableExactApproximateExpressionsView::ContentCell::reloadTextColor() {
  if (numberOfSubviews() == 1+m_displayBurger) {
    m_rightExpressionView.setTextColor(KDColorBlack);
  } else {
    m_rightExpressionView.setTextColor(Palette::GreyVeryDark);
  }
}

KDSize ScrollableExactApproximateExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  KDSize burgerSize = KDSizeZero;
  KDCoordinate width = 0;
  if (m_displayBurger) {
    burgerSize = burgerMenuView()->minimalSizeForOptimalDisplay();
    width += burgerSize.width() + Metric::CommonLargeMargin;
  }
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  width += rightExpressionSize.width();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().isUninitialized() ? 0 : m_rightExpressionView.layout().baseline();
  KDSize leftExpressionSize = KDSizeZero;
  KDCoordinate leftBaseline = 0;
  if (m_displayLeftExpression && !m_leftExpressionView.layout().isUninitialized()) {
    leftBaseline = m_leftExpressionView.layout().baseline();
    leftExpressionSize = m_leftExpressionView.minimalSizeForOptimalDisplay();
    width += leftExpressionSize.width() + 2*Metric::CommonLargeMargin + m_approximateSign.minimalSizeForOptimalDisplay().width();
  }
  KDCoordinate height = maxCoordinate(maxCoordinate(leftBaseline, rightBaseline), burgerSize.height()/2) + maxCoordinate(maxCoordinate(leftExpressionSize.height()-leftBaseline, rightExpressionSize.height()-rightBaseline), burgerSize.height()/2);
  return KDSize(width, height);
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
    assert(m_selectedSubviewPosition == SubviewPosition::Right);
    return m_rightExpressionView.layout();
  }
}

int ScrollableExactApproximateExpressionsView::ContentCell::numberOfSubviews() const {
  int nbOfSubviews = 1;
  if (m_displayLeftExpression && !m_leftExpressionView.layout().isUninitialized()) {
    nbOfSubviews += 2;
  }
  if (m_displayBurger) {
    nbOfSubviews += 1;
  }
  return nbOfSubviews;
}

View * ScrollableExactApproximateExpressionsView::ContentCell::subviewAtIndex(int index) {
  if (m_displayBurger && index == 0) {
    return burgerMenuView();
  }
  View * views[3] = {&m_rightExpressionView, &m_approximateSign, &m_leftExpressionView};
  return views[index - m_displayBurger];
}

void ScrollableExactApproximateExpressionsView::ContentCell::layoutSubviews(bool force) {
  // Subviews sizes
  KDSize burgerSize = m_displayBurger ? burgerMenuView()->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize leftExpressionSize = KDSizeZero;
  KDCoordinate leftBaseline = 0;
  if (m_displayLeftExpression && !m_leftExpressionView.layout().isUninitialized()) {
    leftBaseline = m_leftExpressionView.layout().baseline();
    leftExpressionSize = m_leftExpressionView.minimalSizeForOptimalDisplay();
  }
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().isUninitialized() ? 0 : m_rightExpressionView.layout().baseline();
  // Compute baseline
  KDCoordinate baseline = maxCoordinate(maxCoordinate(leftBaseline, rightBaseline), burgerSize.height()/2);
  // Layout burger
  KDCoordinate currentWidth = 0;
  if (m_displayBurger) {
    burgerMenuView()->setFrame(KDRect(currentWidth, baseline-burgerSize.height()/2, burgerSize), force);
    currentWidth += burgerSize.width() + Metric::CommonLargeMargin;
  }
  // Layout left expression
  if (m_displayLeftExpression && !m_leftExpressionView.layout().isUninitialized()) {
    KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
    m_leftExpressionView.setFrame(KDRect(currentWidth, baseline-leftBaseline, leftExpressionSize), force);
    currentWidth += Metric::CommonLargeMargin+leftExpressionSize.width();
    m_approximateSign.setFrame(KDRect(currentWidth, baseline-approximateSignSize.height()/2, approximateSignSize), force);
    currentWidth += Metric::CommonLargeMargin + approximateSignSize.width();
  }
  // Layout right expression
  m_rightExpressionView.setFrame(KDRect(currentWidth, baseline-rightBaseline, rightExpressionSize), force);
}

BurgerMenuView * ScrollableExactApproximateExpressionsView::ContentCell::burgerMenuView() {
  static BurgerMenuView burger;
  return &burger;
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
  bool burgerIsVisible = false;
  KDCoordinate burgerWidth = 0;
  if (m_contentCell.displayBurger()) {
    burgerWidth = ContentCell::burgerMenuView()->minimalSizeForOptimalDisplay().width();
    burgerIsVisible = burgerWidth - contentOffset().x() > 0;
  }
  KDCoordinate rightExpressionWidth = m_contentCell.rightExpressionView()->minimalSizeForOptimalDisplay().width();
  bool rightExpressionIsVisible = minimalSizeForOptimalDisplay().width() - rightExpressionWidth - contentOffset().x() < bounds().width();
  bool leftExpressionIsVisibleOnTheLeft = false;
  bool leftExpressionIsVisibleOnTheRight = false;
  if (m_contentCell.displayLeftExpression()) {
    KDCoordinate leftExpressionWidth = m_contentCell.leftExpressionView()->minimalSizeForOptimalDisplay().width();
    KDCoordinate signWidth = m_contentCell.approximateSign()->minimalSizeForOptimalDisplay().width();
    leftExpressionIsVisibleOnTheLeft = burgerWidth + Metric::CommonLargeMargin + leftExpressionWidth - contentOffset().x() > 0;
    leftExpressionIsVisibleOnTheRight = minimalSizeForOptimalDisplay().width() - rightExpressionWidth - signWidth - leftExpressionWidth - 2*Metric::CommonLargeMargin - contentOffset().x() < bounds().width();
  }
  // Select left
  if ((event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && leftExpressionIsVisibleOnTheLeft) ||
      (event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Burger && leftExpressionIsVisibleOnTheRight)) {
    setSelectedSubviewPosition(SubviewPosition::Left);
    return true;
  }
  // Select burger
  if ((event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && burgerIsVisible) ||
      (event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Left && burgerIsVisible)) {
    setSelectedSubviewPosition(SubviewPosition::Burger);
    return true;
  }
  if ((event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Left && rightExpressionIsVisible) ||
      (event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Burger && rightExpressionIsVisible)) {
    setSelectedSubviewPosition(SubviewPosition::Right);
    return true;
  }
  return ScrollableView::handleEvent(event);
}

}
