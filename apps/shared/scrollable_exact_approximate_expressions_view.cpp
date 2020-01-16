#include "scrollable_exact_approximate_expressions_view.h"
#include <apps/i18n.h>
#include <assert.h>
using namespace Poincare;

namespace Shared {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

AbstractScrollableExactApproximateExpressionsView::ContentCell::ContentCell() :
  m_rightExpressionView(),
  m_approximateSign(KDFont::LargeFont, I18n::Message::AlmostEqual, 0.5f, 0.5f, Palette::GreyVeryDark),
  m_centeredExpressionView(),
  m_selectedSubviewPosition(SubviewPosition::Center),
  m_displayCenter(true),
  m_displayLeft(false)
{
}

KDColor AbstractScrollableExactApproximateExpressionsView::ContentCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::setHighlighted(bool highlight) {
  // Do not call HighlightCell::setHighlighted to avoid marking all cell as dirty
  m_highlighted = highlight;
  m_centeredExpressionView.setBackgroundColor(backgroundColor());
  m_rightExpressionView.setBackgroundColor(backgroundColor());
  m_approximateSign.setBackgroundColor(backgroundColor());
  if (m_displayLeft) {
    setLeftViewBackgroundColor(backgroundColor());
  }
  if (highlight) {
    if (m_selectedSubviewPosition == SubviewPosition::Center) {
      m_centeredExpressionView.setBackgroundColor(Palette::Select);
    } else if (m_selectedSubviewPosition == SubviewPosition::Right) {
      m_rightExpressionView.setBackgroundColor(Palette::Select);
    } else {
      setLeftViewBackgroundColor(Palette::Select);
    }
  }
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_centeredExpressionView.setBackgroundColor(backgroundColor());
  m_rightExpressionView.setBackgroundColor(backgroundColor());
  m_approximateSign.setBackgroundColor(backgroundColor());
  if (m_displayLeft) {
    setLeftViewBackgroundColor(backgroundColor());
  }
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::reloadTextColor() {
  if (numberOfSubviews() == 1+m_displayLeft) {
    m_rightExpressionView.setTextColor(KDColorBlack);
  } else {
    m_rightExpressionView.setTextColor(Palette::GreyVeryDark);
  }
}

KDSize AbstractScrollableExactApproximateExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  KDSize leftSize = KDSizeZero;
  KDCoordinate leftViewBaseline = 0;
  KDCoordinate width = 0;
  if (m_displayLeft) {
    leftSize = leftMinimalSizeForOptimalDisplay();
    leftViewBaseline = leftBaseline();
    width += leftSize.width() + Metric::CommonLargeMargin;
  }
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  width += rightExpressionSize.width();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().isUninitialized() ? 0 : m_rightExpressionView.layout().baseline();
  KDSize centeredExpressionSize = KDSizeZero;
  KDCoordinate centeredBaseline = 0;
  if (m_displayCenter && !m_centeredExpressionView.layout().isUninitialized()) {
    centeredBaseline = m_centeredExpressionView.layout().baseline();
    centeredExpressionSize = m_centeredExpressionView.minimalSizeForOptimalDisplay();
    width += centeredExpressionSize.width() + 2*Metric::CommonLargeMargin + m_approximateSign.minimalSizeForOptimalDisplay().width();
  }
  KDCoordinate height = maxCoordinate(maxCoordinate(centeredBaseline, rightBaseline), leftViewBaseline) + maxCoordinate(maxCoordinate(centeredExpressionSize.height()-centeredBaseline, rightExpressionSize.height()-rightBaseline), leftSize.height()-leftViewBaseline);
  return KDSize(width, height);
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::setSelectedSubviewPosition(AbstractScrollableExactApproximateExpressionsView::SubviewPosition subviewPosition) {
  m_selectedSubviewPosition = subviewPosition;
  setHighlighted(isHighlighted());
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::setDisplayCenter(bool display) {
  m_displayCenter = display;
  reloadTextColor();
  layoutSubviews();
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::setDisplayLeft(bool display) {
  m_displayLeft = display;
  layoutSubviews();
}

Poincare::Layout AbstractScrollableExactApproximateExpressionsView::ContentCell::layout() const {
  if (m_selectedSubviewPosition == SubviewPosition::Center) {
    return m_centeredExpressionView.layout();
  } else if (m_selectedSubviewPosition == SubviewPosition::Right) {
    return m_rightExpressionView.layout();
  }
  assert(m_selectedSubviewPosition == SubviewPosition::Left);
  return leftLayout();
}

int AbstractScrollableExactApproximateExpressionsView::ContentCell::numberOfSubviews() const {
  int nbOfSubviews = 1;
  if (m_displayCenter && !m_centeredExpressionView.layout().isUninitialized()) {
    nbOfSubviews += 2;
  }
  if (m_displayLeft) {
    nbOfSubviews += 1;
  }
  return nbOfSubviews;
}

View * AbstractScrollableExactApproximateExpressionsView::ContentCell::subviewAtIndex(int index) {
  if (m_displayLeft && index == 0) {
    return leftView();
  }
  View * views[3] = {&m_rightExpressionView, &m_approximateSign, &m_centeredExpressionView};
  return views[index - m_displayLeft];
}

void AbstractScrollableExactApproximateExpressionsView::ContentCell::layoutSubviews(bool force) {
  // Subviews sizes
  KDSize leftSize = m_displayLeft ? leftView()->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDCoordinate leftViewBaseline = m_displayLeft ? leftBaseline() : 0;
  KDSize centeredExpressionSize = KDSizeZero;
  KDCoordinate centeredBaseline = 0;
  if (m_displayCenter && !m_centeredExpressionView.layout().isUninitialized()) {
    centeredBaseline = m_centeredExpressionView.layout().baseline();
    centeredExpressionSize = m_centeredExpressionView.minimalSizeForOptimalDisplay();
  }
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().isUninitialized() ? 0 : m_rightExpressionView.layout().baseline();
  // Compute baseline
  KDCoordinate baseline = maxCoordinate(maxCoordinate(leftViewBaseline, rightBaseline), centeredBaseline);
  // Layout left view
  KDCoordinate currentWidth = 0;
  if (m_displayLeft) {
    leftView()->setFrame(KDRect(currentWidth, baseline-leftViewBaseline, leftSize), force);
    currentWidth += leftSize.width() + Metric::CommonLargeMargin;
  }
  // Layout centered expression
  if (m_displayCenter && !m_centeredExpressionView.layout().isUninitialized()) {
    KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
    m_centeredExpressionView.setFrame(KDRect(currentWidth, baseline-centeredBaseline, centeredExpressionSize), force);
    currentWidth += Metric::CommonLargeMargin+centeredExpressionSize.width();
    m_approximateSign.setFrame(KDRect(currentWidth, baseline-approximateSignSize.height()/2, approximateSignSize), force);
    currentWidth += Metric::CommonLargeMargin + approximateSignSize.width();
  }
  // Layout right expression
  m_rightExpressionView.setFrame(KDRect(currentWidth, baseline-rightBaseline, rightExpressionSize), force);
}

AbstractScrollableExactApproximateExpressionsView::AbstractScrollableExactApproximateExpressionsView(Responder * parentResponder, View * contentCell) :
  ScrollableView(parentResponder, contentCell, this)
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
}

void AbstractScrollableExactApproximateExpressionsView::setLayouts(Poincare::Layout rightLayout, Poincare::Layout leftLayout) {
  bool updateRightLayout = contentCell()->rightExpressionView()->setLayout(rightLayout);
  bool updateLeftLayout = contentCell()->centeredExpressionView()->setLayout(leftLayout);
  if (updateRightLayout || updateLeftLayout) {
    contentCell()->reloadTextColor();
    contentCell()->layoutSubviews();
    reloadScroll();
  }
}

void AbstractScrollableExactApproximateExpressionsView::setEqualMessage(I18n::Message equalSignMessage) {
  contentCell()->approximateSign()->setMessage(equalSignMessage);
}

void AbstractScrollableExactApproximateExpressionsView::reloadScroll() {
  if (selectedSubviewPosition() == SubviewPosition::Right) {
    // Scroll to the right extremity
    scrollToContentPoint(KDPoint(contentCell()->bounds().width(), 0), true);
  } else {
    // Scroll to the left extremity
    ScrollableView::reloadScroll();
  }
}
void AbstractScrollableExactApproximateExpressionsView::setDisplayCenter(bool display) {
  contentCell()->setDisplayCenter(display);
  layoutSubviews();
}

void AbstractScrollableExactApproximateExpressionsView::setDisplayLeft(bool display) {
  contentCell()->setDisplayLeft(display);
  layoutSubviews();
}

bool AbstractScrollableExactApproximateExpressionsView::handleEvent(Ion::Events::Event event) {
  bool leftIsVisible = false;
  KDCoordinate leftWidth = 0;
  if (contentCell()->displayLeft()) {
    leftWidth = contentCell()->leftView()->minimalSizeForOptimalDisplay().width();
    leftIsVisible = leftWidth - contentOffset().x() > 0;
  }
  KDCoordinate rightExpressionWidth = contentCell()->rightExpressionView()->minimalSizeForOptimalDisplay().width();
  bool rightExpressionIsVisible = minimalSizeForOptimalDisplay().width() - rightExpressionWidth - contentOffset().x() < bounds().width();
  bool centeredExpressionIsVisibleOnTheLeft = false;
  bool centeredExpressionIsVisibleOnTheRight = false;
  if (contentCell()->displayCenter()) {
    KDCoordinate centerExpressionWidth = contentCell()->centeredExpressionView()->minimalSizeForOptimalDisplay().width();
    KDCoordinate signWidth = contentCell()->approximateSign()->minimalSizeForOptimalDisplay().width();
    centeredExpressionIsVisibleOnTheLeft = leftWidth + Metric::CommonLargeMargin + centerExpressionWidth - contentOffset().x() > 0;
    centeredExpressionIsVisibleOnTheRight = minimalSizeForOptimalDisplay().width() - rightExpressionWidth - signWidth - centerExpressionWidth - 2*Metric::CommonLargeMargin - contentOffset().x() < bounds().width();
  }
  // Select left
  if ((event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && centeredExpressionIsVisibleOnTheLeft) ||
      (event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Left && centeredExpressionIsVisibleOnTheRight)) {
    setSelectedSubviewPosition(SubviewPosition::Center);
    return true;
  }
  // Select burger
  if ((event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && leftIsVisible) ||
      (event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Center && leftIsVisible)) {
    setSelectedSubviewPosition(SubviewPosition::Left);
    return true;
  }
  if ((event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Center && rightExpressionIsVisible) ||
      (event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Left && rightExpressionIsVisible)) {
    setSelectedSubviewPosition(SubviewPosition::Right);
    return true;
  }
  return ScrollableView::handleEvent(event);
}

BurgerMenuView * ScrollableExactApproximateExpressionsView::ContentCell::burgerMenuView() {
  static BurgerMenuView burger;
  return &burger;
}

}
