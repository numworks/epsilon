#include "scrollable_multiple_expressions_view.h"
#include <apps/i18n.h>
#include <assert.h>
#include <algorithm>
using namespace Poincare;

namespace Shared {

AbstractScrollableMultipleExpressionsView::ContentCell::ContentCell() :
  m_rightExpressionView(),
  m_approximateSign(KDFont::LargeFont, I18n::Message::AlmostEqual, 0.5f, 0.5f, Palette::GreyVeryDark),
  m_centeredExpressionView(),
  m_selectedSubviewPosition(SubviewPosition::Center),
  m_displayCenter(true)
{
}

KDColor AbstractScrollableMultipleExpressionsView::ContentCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}

void AbstractScrollableMultipleExpressionsView::ContentCell::setHighlighted(bool highlight) {
  // Do not call HighlightCell::setHighlighted to avoid marking all cell as dirty
  m_highlighted = highlight;
  KDColor defaultColor = backgroundColor();
  KDColor color = highlight && m_selectedSubviewPosition == SubviewPosition::Center ? Palette::Select : defaultColor;
  m_centeredExpressionView.setBackgroundColor(color);
  color = highlight && m_selectedSubviewPosition == SubviewPosition::Right ? Palette::Select : defaultColor;
  m_rightExpressionView.setBackgroundColor(color);
  m_approximateSign.setBackgroundColor(defaultColor);
  if (leftExpressionView()) {
    color = highlight && m_selectedSubviewPosition == SubviewPosition::Left ? Palette::Select : defaultColor;
    leftExpressionView()->setBackgroundColor(color);
  }
}

void AbstractScrollableMultipleExpressionsView::ContentCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  KDColor defaultColor = backgroundColor();
  m_centeredExpressionView.setBackgroundColor(defaultColor);
  m_rightExpressionView.setBackgroundColor(defaultColor);
  m_approximateSign.setBackgroundColor(defaultColor);
  if (leftExpressionView()) {
    leftExpressionView()->setBackgroundColor(defaultColor);
  }
}

void AbstractScrollableMultipleExpressionsView::ContentCell::reloadTextColor() {
  if (displayCenter()) {
    m_rightExpressionView.setTextColor(Palette::GreyVeryDark);
  } else {
    m_rightExpressionView.setTextColor(KDColorBlack);
  }
}

KDSize AbstractScrollableMultipleExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  KDSize leftSize = KDSizeZero;
  KDCoordinate leftViewBaseline = 0;
  KDCoordinate width = 0;
  if (leftExpressionView() && !leftExpressionView()->layout().isUninitialized()) {
    leftSize = leftExpressionView()->minimalSizeForOptimalDisplay();
    leftViewBaseline = leftExpressionView()->layout().baseline();
    width += leftSize.width() + Metric::CommonLargeMargin;
  }
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  width += rightExpressionSize.width();
  Layout l = m_rightExpressionView.layout();
  KDCoordinate rightBaseline = l.isUninitialized() ? 0 : l.baseline();
  KDSize centeredExpressionSize = KDSizeZero;
  KDCoordinate centeredBaseline = 0;
  if (displayCenter()) {
    centeredBaseline = m_centeredExpressionView.layout().baseline();
    centeredExpressionSize = m_centeredExpressionView.minimalSizeForOptimalDisplay();
    width += centeredExpressionSize.width() + 2*Metric::CommonLargeMargin + m_approximateSign.minimalSizeForOptimalDisplay().width();
  }
  KDCoordinate height = std::max(std::max(centeredBaseline, rightBaseline), leftViewBaseline) + std::max(std::max(centeredExpressionSize.height()-centeredBaseline, rightExpressionSize.height()-rightBaseline), leftSize.height()-leftViewBaseline);
  return KDSize(width, height);
}

void AbstractScrollableMultipleExpressionsView::ContentCell::setSelectedSubviewPosition(AbstractScrollableMultipleExpressionsView::SubviewPosition subviewPosition) {
  m_selectedSubviewPosition = subviewPosition;
  setHighlighted(isHighlighted());
}

void AbstractScrollableMultipleExpressionsView::ContentCell::setDisplayCenter(bool display) {
  m_displayCenter = display;
  reloadTextColor();
  layoutSubviews();
}

Poincare::Layout AbstractScrollableMultipleExpressionsView::ContentCell::layout() const {
  if (m_selectedSubviewPosition == SubviewPosition::Center) {
    return m_centeredExpressionView.layout();
  } else if (m_selectedSubviewPosition == SubviewPosition::Right) {
    return m_rightExpressionView.layout();
  }
  assert(m_selectedSubviewPosition == SubviewPosition::Left);
  assert(leftExpressionView());
  return leftExpressionView()->layout();
}

int AbstractScrollableMultipleExpressionsView::ContentCell::numberOfSubviews() const {
  int nbOfSubviews = 1;
  if (displayCenter()) {
    nbOfSubviews += 2;
  }
  if (leftExpressionView()) {
    nbOfSubviews += 1;
  }
  return nbOfSubviews;
}

View * AbstractScrollableMultipleExpressionsView::ContentCell::subviewAtIndex(int index) {
  bool leftIsVisible = leftExpressionView() != nullptr;
  if (leftIsVisible && index == 0) {
    return leftExpressionView();
  }
  View * views[3] = {&m_rightExpressionView, &m_approximateSign, &m_centeredExpressionView};
  return views[index - leftIsVisible];
}

void AbstractScrollableMultipleExpressionsView::ContentCell::layoutSubviews(bool force) {
  // Subviews sizes
  KDSize leftSize = leftExpressionView() ? leftExpressionView()->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDCoordinate leftViewBaseline = leftExpressionView() && !leftExpressionView()->layout().isUninitialized() ? leftExpressionView()->layout().baseline() : 0;
  KDSize centeredExpressionSize = KDSizeZero;
  KDCoordinate centeredBaseline = 0;
  if (displayCenter()) {
    centeredBaseline = m_centeredExpressionView.layout().baseline();
    centeredExpressionSize = m_centeredExpressionView.minimalSizeForOptimalDisplay();
  }
  KDSize rightExpressionSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
  KDCoordinate rightBaseline = m_rightExpressionView.layout().isUninitialized() ? 0 : m_rightExpressionView.layout().baseline();
  // Compute baseline
  KDCoordinate baseline = std::max(std::max(leftViewBaseline, rightBaseline), centeredBaseline);
  // Layout left view
  KDCoordinate currentWidth = 0;
  if (leftExpressionView()) {
    leftExpressionView()->setFrame(KDRect(currentWidth, baseline-leftViewBaseline, leftSize), force);
    currentWidth += leftSize.width() + Metric::CommonLargeMargin;
  }
  // Layout centered expression
  if (displayCenter()) {
    KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
    m_centeredExpressionView.setFrame(KDRect(currentWidth, baseline-centeredBaseline, centeredExpressionSize), force);
    currentWidth += Metric::CommonLargeMargin+centeredExpressionSize.width();
    m_approximateSign.setFrame(KDRect(currentWidth, baseline-approximateSignSize.height()/2, approximateSignSize), force);
    currentWidth += Metric::CommonLargeMargin + approximateSignSize.width();
  }
  // Layout right expression
  m_rightExpressionView.setFrame(KDRect(currentWidth, baseline-rightBaseline, rightExpressionSize), force);
}

AbstractScrollableMultipleExpressionsView::AbstractScrollableMultipleExpressionsView(Responder * parentResponder, View * contentCell) :
  ScrollableView(parentResponder, contentCell, this)
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
}

void AbstractScrollableMultipleExpressionsView::setLayouts(Poincare::Layout leftLayout, Poincare::Layout centerLayout, Poincare::Layout rightLayout) {
  bool updateRightLayout = contentCell()->rightExpressionView()->setLayout(rightLayout);
  bool updateCenterLayout = contentCell()->centeredExpressionView()->setLayout(centerLayout);
  bool updateLeftLayout = false;
  if (contentCell()->leftExpressionView()) {
    updateLeftLayout = contentCell()->leftExpressionView()->setLayout(leftLayout);
  }
  if (updateLeftLayout || updateCenterLayout || updateRightLayout) {
    contentCell()->reloadTextColor();
    // Reload expressions layouts
    contentCell()->layoutSubviews();
    // Reload the scroll content view layout (the content size might have changed)
    layoutSubviews();
    // Do no reload scroll here as 'setLayouts' is called every time the table is re-layout (when scrolling for instance)
  }
}

void AbstractScrollableMultipleExpressionsView::setEqualMessage(I18n::Message equalSignMessage) {
  contentCell()->approximateSign()->setMessage(equalSignMessage);
}

void AbstractScrollableMultipleExpressionsView::reloadScroll() {
  if (selectedSubviewPosition() == SubviewPosition::Right) {
    // Scroll to the right extremity
    scrollToContentPoint(KDPoint(contentCell()->bounds().width(), 0), true);
  } else {
    // Scroll to the left extremity
    ScrollableView::reloadScroll();
  }
}
void AbstractScrollableMultipleExpressionsView::setDisplayCenter(bool display) {
  contentCell()->setDisplayCenter(display);
  layoutSubviews();
}

bool AbstractScrollableMultipleExpressionsView::handleEvent(Ion::Events::Event event) {
  bool leftIsVisible = false;
  KDCoordinate leftWidth = 0;
  if (contentCell()->leftExpressionView()) {
    leftWidth = contentCell()->leftExpressionView()->minimalSizeForOptimalDisplay().width();
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
  // Select center
  if ((event == Ion::Events::Left && selectedSubviewPosition() == SubviewPosition::Right && centeredExpressionIsVisibleOnTheLeft) ||
      (event == Ion::Events::Right && selectedSubviewPosition() == SubviewPosition::Left && centeredExpressionIsVisibleOnTheRight)) {
    setSelectedSubviewPosition(SubviewPosition::Center);
    return true;
  }
  // Select left
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

}
