#include "scrollable_multiple_expressions_view.h"
#include <apps/i18n.h>
#include <assert.h>
#include <algorithm>

using namespace Escher;
using namespace Poincare;

namespace Shared {

AbstractScrollableMultipleExpressionsView::ContentCell::ContentCell(float horizontalAlignment, KDFont::Size font) :
  m_rightExpressionView(KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite, font),
  m_approximateSign(font, k_defaultApproximateMessage, KDContext::k_alignCenter,
                    KDContext::k_alignCenter, Palette::GrayVeryDark),
  m_centeredExpressionView(KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite, font),
  m_selectedSubviewPosition(SubviewPosition::Center),
  m_displayCenter(true),
  m_horizontalAlignment(horizontalAlignment)
{
}

KDColor AbstractScrollableMultipleExpressionsView::ContentCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}

void AbstractScrollableMultipleExpressionsView::ContentCell::setHighlighted(bool highlight) {
  // Do not call HighlightCell::setHighlighted to avoid marking all cell as dirty
  setHighlightedWithoutReload(highlight);
  updateSubviewsBackgroundAfterChangingState();
}

void AbstractScrollableMultipleExpressionsView::ContentCell::updateSubviewsBackgroundAfterChangingState() {
  KDColor defaultColor = backgroundColor();
  bool highlight = isHighlighted();
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

void AbstractScrollableMultipleExpressionsView::ContentCell::reloadTextColor() {
  m_rightExpressionView.setTextColor(displayCenter() ? Palette::GrayVeryDark : KDColorBlack);
}

KDSize AbstractScrollableMultipleExpressionsView::ContentCell::minimalSizeForOptimalDisplay() const {
  return privateMinimalSizeForOptimalDisplay(false);
}

KDSize AbstractScrollableMultipleExpressionsView::ContentCell::minimalSizeForOptimalDisplayFullSize() const {
  return privateMinimalSizeForOptimalDisplay(true);
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

void AbstractScrollableMultipleExpressionsView::ContentCell::setRightIsStrictlyEqual(bool isEqual) {
  m_rightIsStrictlyEqual = isEqual;
  approximateSign()->setMessage(isEqual ? I18n::Message::Equal : I18n::Message::AlmostEqual);
  reloadTextColor();
}

Poincare::Layout AbstractScrollableMultipleExpressionsView::ContentCell::layoutAtPosition(SubviewPosition position) const {
  if (position == SubviewPosition::Center) {
    return m_centeredExpressionView.layout();
  } else if (position == SubviewPosition::Right) {
    return m_rightExpressionView.layout();
  }
  assert(position == SubviewPosition::Left);
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

KDCoordinate AbstractScrollableMultipleExpressionsView::ContentCell::baseline(KDCoordinate * leftBaseline, KDCoordinate * centerBaseline, KDCoordinate * rightBaseline) const {
  // Left view
  KDCoordinate leftViewBaseline = (leftExpressionView() && !leftExpressionView()->layout().isUninitialized()) ?
    leftExpressionView()->layout().baseline(leftExpressionView()->font()) :
    0;
  if (leftBaseline != nullptr) {
    *leftBaseline = leftViewBaseline;
  }

  // Center view
  KDCoordinate centerViewBaseline = displayCenter() ? m_centeredExpressionView.layout().baseline(font()) : 0;
  if (centerBaseline != nullptr) {
    *centerBaseline = centerViewBaseline;
  }

  // Right view
  KDCoordinate rightViewBaseline = m_rightExpressionView.layout().isUninitialized() ?
    0 :
    m_rightExpressionView.layout().baseline(font());
  if (rightBaseline != nullptr) {
    *rightBaseline = rightViewBaseline;
  }

  return std::max({leftViewBaseline, centerViewBaseline, rightViewBaseline});
}

void AbstractScrollableMultipleExpressionsView::ContentCell::subviewFrames(KDRect * leftFrame, KDRect * centerFrame, KDRect * approximateSignFrame, KDRect * rightFrame) {
  // Subviews sizes
  KDSize leftSize = leftExpressionView() ? leftExpressionView()->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize centerSize = displayCenter() ? m_centeredExpressionView.minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize rightSize = m_rightExpressionView.minimalSizeForOptimalDisplay();

  // Compute baselines
  KDCoordinate leftBaseline = 0;
  KDCoordinate centerBaseline = 0;
  KDCoordinate rightBaseline = 0;
  KDCoordinate viewBaseline = baseline(&leftBaseline, &centerBaseline, &rightBaseline);

  // Layout left view
  KDCoordinate currentWidth = 0;
  if (leftExpressionView() && !leftExpressionView()->layout().isUninitialized()) {
    assert(leftFrame != nullptr);
    *leftFrame = KDRect(currentWidth, viewBaseline - leftBaseline, leftSize);
    currentWidth += leftSize.width() + AbstractScrollableMultipleExpressionsView::k_horizontalMargin;
  }

  // Layout center expression
  if (displayCenter()) {
    assert(centerFrame != nullptr && approximateSignFrame != nullptr);
    KDSize approximateSignSize = m_approximateSign.minimalSizeForOptimalDisplay();
    *centerFrame = KDRect(currentWidth, viewBaseline - centerBaseline, centerSize);
    currentWidth += AbstractScrollableMultipleExpressionsView::k_horizontalMargin + centerSize.width();
    *approximateSignFrame = displayApproximateSign() ? KDRect(currentWidth, viewBaseline - approximateSignSize.height()/2, approximateSignSize) : KDRectZero;
    currentWidth += AbstractScrollableMultipleExpressionsView::k_horizontalMargin + approximateSignSize.width();
  }

  // Layout right expression
  assert(rightFrame != nullptr);
  *rightFrame = KDRect(currentWidth, viewBaseline - rightBaseline, rightSize);
  currentWidth += rightSize.width();

  /* Handle horizontal alignment right, left or center. Vertical alignment
   * could be handled here too but there is no use for implementing it
   * for now. */
  if (currentWidth < m_frame.width()) {
    KDCoordinate horizontalOffset = currentWidth < bounds().width() ? std::round((bounds().width() - currentWidth) * m_horizontalAlignment) : 0;
    KDPoint offset = KDPoint(horizontalOffset, 0);
    *rightFrame = rightFrame->translatedBy(offset);
    if (leftExpressionView()) {
      *leftFrame = leftFrame->translatedBy(offset);
    }
    if (displayCenter()) {
      *centerFrame = centerFrame->translatedBy(offset);
      *approximateSignFrame = approximateSignFrame->translatedBy(offset);
    }
  }
}

KDSize AbstractScrollableMultipleExpressionsView::ContentCell::privateMinimalSizeForOptimalDisplay(bool forceFullDisplay) const {
  KDCoordinate width = 0;
  KDCoordinate height = 0;

  KDSize leftSize = KDSizeZero;
  if (leftExpressionView() && !leftExpressionView()->layout().isUninitialized()) {
    leftSize = leftExpressionView()->minimalSizeForOptimalDisplay();
    width += leftSize.width() + AbstractScrollableMultipleExpressionsView::k_horizontalMargin;
    height = std::max(height, leftSize.height());

  }

  KDSize centerSize = KDSizeZero;
  if (displayCenter() || (forceFullDisplay && displayableCenter())) {
    centerSize = m_centeredExpressionView.minimalSizeForOptimalDisplay();
    width += centerSize.width() + 2 * AbstractScrollableMultipleExpressionsView::k_horizontalMargin + m_approximateSign.minimalSizeForOptimalDisplay().width();
    height = std::max(height, centerSize.height());
  }

  KDSize rightSize = KDSizeZero;
  if (!m_rightExpressionView.layout().isUninitialized()) {
    rightSize = m_rightExpressionView.minimalSizeForOptimalDisplay();
    width += rightSize.width();
  }

  height = std::max(height, rightSize.height());

  return KDSize(width, height);
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
  if (bounds().width() <= 0 || bounds().height() <= 0) {
    // TODO Make this behaviour in a non-virtual layoutSublviews, and all layout subviews should become privateLayoutSubviews
    return;
  }
  KDRect leftFrame = KDRectZero;
  KDRect centerFrame = KDRectZero;
  KDRect approximateSignFrame = KDRectZero;
  KDRect rightFrame = KDRectZero;
  subviewFrames(&leftFrame, &centerFrame, &approximateSignFrame, &rightFrame);
  if (leftExpressionView() != nullptr) {
    leftExpressionView()->setFrame(leftFrame, force);
  }
  if (centeredExpressionView() != nullptr) {
    centeredExpressionView()->setFrame(centerFrame, force);
  }
  m_approximateSign.setFrame(approximateSignFrame, force);
  if (rightExpressionView() != nullptr) {
    rightExpressionView()->setFrame(rightFrame, force);
  }
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
    /* TODO revert commit 87e48361961d1?
     * We can call reloadScroll here instead of in didBecome firstResponder,
     * because we fixed setLayouts (updateLeftLayout, updateCenterLayout and
     * updateRightLayout are now sometimes false). */
  }
}

void AbstractScrollableMultipleExpressionsView::reloadScroll() {
  if (selectedSubviewPosition() == SubviewPosition::Right) {
    /* Scroll to the right extremity, then back to the beginning of the right
     * expression. This ensures that the beginning of the expression is in
     * frame, and that as much of it as possible is visible. */
    scrollToContentPoint(KDPoint(contentCell()->bounds().width(), 0), true);
    scrollToContentPoint(KDPoint(contentCell()->bounds().width() - contentCell()->rightExpressionView()->bounds().width(), 0), true);
  } else {
    /* Scroll to the left extremity. */
    ScrollableView::reloadScroll();
  }
}
void AbstractScrollableMultipleExpressionsView::setDisplayCenter(bool display) {
  contentCell()->setDisplayCenter(display);
  layoutSubviews();
}

bool AbstractScrollableMultipleExpressionsView::handleEvent(Ion::Events::Event & event) {
  if (event == Ion::Events::Left || event == Ion::Events::Right ) {
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
      centeredExpressionIsVisibleOnTheLeft = leftWidth + k_horizontalMargin + centerExpressionWidth - contentOffset().x() > 0;
      centeredExpressionIsVisibleOnTheRight = minimalSizeForOptimalDisplay().width() - rightExpressionWidth - signWidth - centerExpressionWidth - 2 * k_horizontalMargin - contentOffset().x() < bounds().width();
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
  }
  return ScrollableView::handleEvent(event);
}

KDSize ScrollableTwoExpressionsView::minimalSizeForOptimalDisplayFullSize() const {
  KDSize res = contentCell()->minimalSizeForOptimalDisplayFullSize();
  return KDSize(res.width() + leftMargin() + rightMargin(), res.height() + topMargin() + bottomMargin());
}

}
