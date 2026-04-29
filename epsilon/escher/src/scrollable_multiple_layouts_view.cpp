#include <assert.h>
#include <escher/scrollable_multiple_layouts_view.h>
#include <omg/unreachable.h>

#include <algorithm>
#include <cmath>

using namespace Poincare;

namespace Escher {

// TODO: move the whole file to apps/shared and use i18n ?
const char* k_almostEqual = "≈";
const char* k_equal = "=";

AbstractScrollableMultipleLayoutsView::ContentCell::ContentCell(
    float horizontalAlignment, KDFont::Size font)
    : m_rightLayoutView({.style = {.font = font}}),
      m_comparisonSign(
          {.style = {.glyphColor = ThemeComponent::TextGrayVeryDark,
                     .font = font},
           .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_centeredLayoutView({.style = {.font = font}}),
      m_selectedSubviewPosition(SubviewPosition::Center),
      m_displayCenter(true),
      m_toggleableCenter(false),
      m_comparisonSignIsEqual(false),
      m_displayComparisonSign(true),
      m_horizontalAlignment(horizontalAlignment) {
  m_comparisonSign.setText(k_almostEqual);
}

ThemeComponent
AbstractScrollableMultipleLayoutsView::ContentCell::backgroundColor() const {
  ThemeComponent background =
      m_even ? ThemeComponent::BackgroundWhite : ThemeComponent::WallScreen;
  return background;
}

void AbstractScrollableMultipleLayoutsView::ContentCell::setHighlighted(
    bool highlight) {
  /* Do not call HighlightCell::setHighlighted to avoid marking all cell as
   * dirty */
  setHighlightedWithoutReload(highlight);
  updateSubviewsBackgroundAfterChangingState();
}

void AbstractScrollableMultipleLayoutsView::ContentCell::
    updateSubviewsBackgroundAfterChangingState() {
  ThemeComponent defaultColor = backgroundColor();
  bool highlight = isHighlighted();

  bool centerIsHighlighted =
      highlight && m_selectedSubviewPosition == SubviewPosition::Center;
  m_centeredLayoutView.setHighlighted(centerIsHighlighted);
  m_centeredLayoutView.setBackgroundColor(
      centerIsHighlighted ? ThemeComponent::SurfaceSelect : defaultColor);

  bool rightIsHighLighted =
      highlight && m_selectedSubviewPosition == SubviewPosition::Right;
  m_rightLayoutView.setHighlighted(rightIsHighLighted);
  m_rightLayoutView.setBackgroundColor(
      rightIsHighLighted ? ThemeComponent::SurfaceSelect : defaultColor);

  m_comparisonSign.setBackgroundColor(defaultColor);

  if (displayLeftLayout()) {
    bool leftIsHighLighted =
        highlight && m_selectedSubviewPosition == SubviewPosition::Left;
    leftLayoutView()->setHighlighted(leftIsHighLighted);
    leftLayoutView()->setBackgroundColor(
        leftIsHighLighted ? ThemeComponent::SurfaceSelect : defaultColor);
  }
}

void AbstractScrollableMultipleLayoutsView::ContentCell::reloadTextColor() {
  ThemeComponent color = displayCenter() ? ThemeComponent::TextGrayVeryDark
                                         : ThemeComponent::TextDefault;
  m_comparisonSign.setTextColor(color);
  m_rightLayoutView.setTextColor(color);
}

KDSize AbstractScrollableMultipleLayoutsView::ContentCell::
    minimalSizeForOptimalDisplay() const {
  return privateMinimalSizeForOptimalDisplay(false);
}

KDSize AbstractScrollableMultipleLayoutsView::ContentCell::
    minimalSizeForOptimalDisplayFullSize() const {
  return privateMinimalSizeForOptimalDisplay(true);
}

void AbstractScrollableMultipleLayoutsView::ContentCell::
    setSelectedSubviewPosition(
        AbstractScrollableMultipleLayoutsView::SubviewPosition
            subviewPosition) {
  m_selectedSubviewPosition = subviewPosition;
  setHighlighted(isHighlighted());
}

void AbstractScrollableMultipleLayoutsView::ContentCell::setDisplayCenter(
    bool display) {
  m_displayCenter = display;
  reloadTextColor();
  layoutSubviews();
}

void AbstractScrollableMultipleLayoutsView::ContentCell::setComparisonSign(
    bool isEqual, bool isVisible) {
  m_comparisonSignIsEqual = isEqual;
  m_displayComparisonSign = isVisible;
  comparisonSign()->setText(isEqual ? k_equal : k_almostEqual);
  reloadTextColor();
}

const Layout&
AbstractScrollableMultipleLayoutsView::ContentCell::layoutAtPosition(
    SubviewPosition position) const {
  if (position == SubviewPosition::Center) {
    return m_centeredLayoutView.layout();
  } else if (position == SubviewPosition::Right) {
    return m_rightLayoutView.layout();
  }
  assert(position == SubviewPosition::Left);
  assert(displayLeftLayout());
  return leftLayoutView()->layout();
}

int AbstractScrollableMultipleLayoutsView::ContentCell::numberOfSubviews()
    const {
  return displayLeftLayout() + displayCenter() + displayComparisonSign() + 1;
}

KDCoordinate AbstractScrollableMultipleLayoutsView::ContentCell::baseline(
    KDCoordinate* leftBaseline, KDCoordinate* centerBaseline,
    KDCoordinate* rightBaseline) const {
  // Left view
  KDCoordinate leftViewBaseline =
      displayLeftLayout()
          ? leftLayoutView()->layout()->baseline(leftLayoutView()->font())
          : 0;
  if (leftBaseline != nullptr) {
    *leftBaseline = leftViewBaseline;
  }

  // Center view
  KDCoordinate centerViewBaseline =
      displayCenter() ? m_centeredLayoutView.layout()->baseline(font()) : 0;
  if (centerBaseline != nullptr) {
    *centerBaseline = centerViewBaseline;
  }

  // Right view
  KDCoordinate rightViewBaseline =
      displayRightLayout() ? m_rightLayoutView.layout()->baseline(font()) : 0;
  if (rightBaseline != nullptr) {
    *rightBaseline = rightViewBaseline;
  }

  return std::max({leftViewBaseline, centerViewBaseline, rightViewBaseline});
}

void AbstractScrollableMultipleLayoutsView::ContentCell::subviewFrames(
    KDRect* leftFrame, KDRect* centerFrame, KDRect* comparisonSignFrame,
    KDRect* rightFrame) {
  // Subviews sizes
  KDSize leftSize = displayLeftLayout()
                        ? leftLayoutView()->minimalSizeForOptimalDisplay()
                        : KDSizeZero;
  KDSize centerSize = displayCenter()
                          ? m_centeredLayoutView.minimalSizeForOptimalDisplay()
                          : KDSizeZero;
  KDSize rightSize = m_rightLayoutView.minimalSizeForOptimalDisplay();

  // Compute baselines
  KDCoordinate leftBaseline = 0;
  KDCoordinate centerBaseline = 0;
  KDCoordinate rightBaseline = 0;
  KDCoordinate viewBaseline =
      baseline(&leftBaseline, &centerBaseline, &rightBaseline);

  // Layout left view
  KDCoordinate currentWidth = 0;
  if (displayLeftLayout()) {
    assert(leftFrame != nullptr);
    *leftFrame = KDRect(currentWidth, viewBaseline - leftBaseline, leftSize);
    currentWidth += leftSize.width() +
                    AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
  }

  // Layout center layout
  KDSize comparisonSignSize = m_comparisonSign.minimalSizeForOptimalDisplay();
  if (displayCenter()) {
    assert(centerFrame != nullptr && comparisonSignFrame != nullptr);
    *centerFrame =
        KDRect(currentWidth, viewBaseline - centerBaseline, centerSize);
    currentWidth += AbstractScrollableMultipleLayoutsView::k_horizontalMargin +
                    centerSize.width();
  }

  // Layout comparison sign
  if (displayComparisonSign()) {
    *comparisonSignFrame =
        KDRect(currentWidth, viewBaseline - comparisonSignSize.height() / 2,
               comparisonSignSize);
    currentWidth += AbstractScrollableMultipleLayoutsView::k_horizontalMargin +
                    comparisonSignSize.width();
  }

  // Layout right layout
  assert(rightFrame != nullptr);
  *rightFrame = KDRect(currentWidth, viewBaseline - rightBaseline, rightSize);
  currentWidth += rightSize.width();

  /* Handle horizontal alignment right, left or center. Vertical alignment
   * could be handled here too but there is no use for implementing it
   * for now. */
  if (currentWidth < bounds().width()) {
    KDCoordinate horizontalOffset =
        currentWidth < bounds().width()
            ? std::round((bounds().width() - currentWidth) *
                         m_horizontalAlignment)
            : 0;
    KDPoint offset = KDPoint(horizontalOffset, 0);
    *rightFrame = rightFrame->translatedBy(offset);
    if (displayLeftLayout()) {
      *leftFrame = leftFrame->translatedBy(offset);
    }
    if (displayCenter()) {
      *centerFrame = centerFrame->translatedBy(offset);
    }
    if (displayComparisonSign()) {
      *comparisonSignFrame = comparisonSignFrame->translatedBy(offset);
    }
  }
}

KDSize AbstractScrollableMultipleLayoutsView::ContentCell::
    privateMinimalSizeForOptimalDisplay(bool forceFullDisplay) const {
  KDCoordinate width = 0;
  KDCoordinate maxAboveBaseline = 0;
  KDCoordinate maxUnderBaseline = 0;

  KDCoordinate leftBaseline = 0;
  KDCoordinate centerBaseline = 0;
  KDCoordinate rightBaseline = 0;
  baseline(&leftBaseline, &centerBaseline, &rightBaseline);

  KDSize leftSize = KDSizeZero;
  if (displayLeftLayout()) {
    leftSize = leftLayoutView()->minimalSizeForOptimalDisplay();
    width += leftSize.width() +
             AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
    maxAboveBaseline = std::max(maxAboveBaseline, leftBaseline);
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline,
                                              leftSize.height() - leftBaseline);
  }

  KDSize centerSize = KDSizeZero;
  /* Although center layout isn't visible, we sometimes need to account for its
   * size to ensure consistent layouting when toggling display of the center
   * layout. */
  bool accountForCenter = forceFullDisplay && displayableCenter();
  if (displayCenter() || accountForCenter) {
    centerSize = m_centeredLayoutView.minimalSizeForOptimalDisplay();
    width += centerSize.width() +
             AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
    maxAboveBaseline = std::max(maxAboveBaseline, centerBaseline);
    maxUnderBaseline = std::max<KDCoordinate>(
        maxUnderBaseline, centerSize.height() - centerBaseline);
  }

  /* NOTE: A displayable center with hidden comparisonSign isn't handled. It is
   * assumed that if [accountForCenter] is true, [displayComparisonSign()] will
   * be true when [displayCenter()] will be true. */
  assert(!(displayCenter() && displayableCenter() && !displayComparisonSign()));
  if (displayComparisonSign() || accountForCenter) {
    width += m_comparisonSign.minimalSizeForOptimalDisplay().width() +
             AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
  }

  KDSize rightSize = KDSizeZero;
  if (displayRightLayout()) {
    rightSize = m_rightLayoutView.minimalSizeForOptimalDisplay();
    width += rightSize.width();
  }
  maxAboveBaseline = std::max(maxAboveBaseline, rightBaseline);
  maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline,
                                            rightSize.height() - rightBaseline);

  KDCoordinate height = maxAboveBaseline + maxUnderBaseline;

  return KDSize(width, height);
}

View* AbstractScrollableMultipleLayoutsView::ContentCell::subviewAtIndex(
    int index) {
  View* subviews[] = {
      displayLeftLayout() ? leftLayoutView() : nullptr,
      displayCenter() ? centeredLayoutView() : nullptr,
      displayComparisonSign() ? comparisonSign() : nullptr,
      rightLayoutView(),
  };
  for (View* subview : subviews) {
    if (subview != nullptr && index-- == 0) {
      return subview;
    }
  }
  OMG_UNREACHABLE;
  return nullptr;
}

void AbstractScrollableMultipleLayoutsView::ContentCell::layoutSubviews(
    bool force) {
  if (bounds().width() <= 0 || bounds().height() <= 0) {
    /* TODO Make this behaviour in a non-virtual layoutSubviews, and all layout
     * subviews should become privateLayoutSubviews */
    return;
  }
  KDRect leftFrame = KDRectZero;
  KDRect centerFrame = KDRectZero;
  KDRect comparisonSignFrame = KDRectZero;
  KDRect rightFrame = KDRectZero;
  subviewFrames(&leftFrame, &centerFrame, &comparisonSignFrame, &rightFrame);
  if (displayLeftLayout()) {
    setChildFrame(leftLayoutView(), leftFrame, force);
  }
  if (centeredLayoutView() != nullptr) {
    setChildFrame(centeredLayoutView(), centerFrame, force);
  }
  setChildFrame(&m_comparisonSign, comparisonSignFrame, force);
  if (rightLayoutView() != nullptr) {
    setChildFrame(rightLayoutView(), rightFrame, force);
  }
}

AbstractScrollableMultipleLayoutsView::AbstractScrollableMultipleLayoutsView(
    Responder* parentResponder, View* contentCell)
    : ScrollableView(parentResponder, contentCell, this) {}

void AbstractScrollableMultipleLayoutsView::setLayouts(
    const Layout& formulaLayout, const Layout& exactLayout,
    const Layout& approximateLayout) {
  bool updateRightLayout =
      contentCell()->rightLayoutView()->setLayout(approximateLayout);
  bool updateCenterLayout =
      contentCell()->centeredLayoutView()->setLayout(exactLayout);
  bool updateLeftLayout = false;
  if (contentCell()->canHaveLeftLayout()) {
    updateLeftLayout =
        contentCell()->leftLayoutView()->setLayout(formulaLayout);
  }
  if (updateLeftLayout || updateCenterLayout || updateRightLayout) {
    contentCell()->reloadTextColor();
    // Reload layouts layouts
    contentCell()->layoutSubviews();
    /* Reload the scroll content view layout (the content size might have
     * changed) */
    layoutSubviews();
    /* TODO revert commit 87e48361961d1?
     * We can call reloadScroll here instead of in didBecome firstResponder,
     * because we fixed setLayouts (updateLeftLayout, updateCenterLayout and
     * updateRightLayout are now sometimes false). */
  }
}

void AbstractScrollableMultipleLayoutsView::reloadScroll() {
  if (selectedSubviewPosition() == SubviewPosition::Right &&
      (contentCell()->displayLeftLayout() || contentCell()->displayCenter())) {
    /* Scroll to the right extremity, then back to the beginning of the right
     * layout. This ensures that the beginning of the layout is in
     * frame, and that as much of it as possible is visible. */
    scrollToContentPoint(KDPoint(contentCell()->bounds().width(), 0));
    scrollToContentPoint(
        KDPoint(contentCell()->bounds().width() -
                    contentCell()->rightLayoutView()->bounds().width(),
                0));
  } else {
    /* Scroll to the left extremity. */
    resetScroll();
  }
}

void AbstractScrollableMultipleLayoutsView::setDisplayCenter(bool display) {
  contentCell()->setDisplayCenter(display);
  layoutSubviews();
}

bool AbstractScrollableMultipleLayoutsView::handleEvent(
    Ion::Events::Event event) {
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    bool leftIsVisible = false;
    KDCoordinate leftWidth = 0;
    if (contentCell()->displayLeftLayout()) {
      leftWidth = contentCell()
                      ->leftLayoutView()
                      ->minimalSizeForOptimalDisplay()
                      .width();
      leftIsVisible = leftWidth - contentOffset().x() > 0;
    }
    KDCoordinate rightLayoutWidth = contentCell()
                                        ->rightLayoutView()
                                        ->minimalSizeForOptimalDisplay()
                                        .width();
    bool rightLayoutIsVisible = minimalSizeForOptimalDisplay().width() -
                                    rightLayoutWidth - contentOffset().x() <
                                bounds().width();
    bool centeredLayoutIsVisibleOnTheLeft = false;
    bool centeredLayoutIsVisibleOnTheRight = false;
    if (contentCell()->displayCenter()) {
      KDCoordinate centerLayoutWidth = contentCell()
                                           ->centeredLayoutView()
                                           ->minimalSizeForOptimalDisplay()
                                           .width();
      KDCoordinate signWidth = contentCell()
                                   ->comparisonSign()
                                   ->minimalSizeForOptimalDisplay()
                                   .width();
      centeredLayoutIsVisibleOnTheLeft = leftWidth + k_horizontalMargin +
                                             centerLayoutWidth -
                                             contentOffset().x() >
                                         0;
      centeredLayoutIsVisibleOnTheRight =
          minimalSizeForOptimalDisplay().width() - rightLayoutWidth -
              signWidth - centerLayoutWidth - 2 * k_horizontalMargin -
              contentOffset().x() <
          bounds().width();
    }
    // Select center
    if ((event == Ion::Events::Left &&
         selectedSubviewPosition() == SubviewPosition::Right &&
         centeredLayoutIsVisibleOnTheLeft) ||
        (event == Ion::Events::Right &&
         selectedSubviewPosition() == SubviewPosition::Left &&
         centeredLayoutIsVisibleOnTheRight)) {
      setSelectedSubviewPosition(SubviewPosition::Center);
      reloadScroll();
      return true;
    }
    // Select left
    if ((event == Ion::Events::Left &&
         selectedSubviewPosition() == SubviewPosition::Right &&
         leftIsVisible) ||
        (event == Ion::Events::Left &&
         selectedSubviewPosition() == SubviewPosition::Center &&
         leftIsVisible)) {
      setSelectedSubviewPosition(SubviewPosition::Left);
      reloadScroll();
      return true;
    }
    if ((event == Ion::Events::Right &&
         selectedSubviewPosition() == SubviewPosition::Center &&
         rightLayoutIsVisible) ||
        (event == Ion::Events::Right &&
         selectedSubviewPosition() == SubviewPosition::Left &&
         rightLayoutIsVisible)) {
      setSelectedSubviewPosition(SubviewPosition::Right);
      reloadScroll();
      return true;
    }
  }
  return ScrollableView::handleEvent(event);
}

void AbstractScrollableMultipleLayoutsView::setHighlighted(bool highlighted) {
  setBackgroundColor(AbstractMenuCell::BackgroundColor(highlighted));
}

KDSize ScrollableTwoLayoutsView::minimalSizeForOptimalDisplayFullSize() const {
  return contentCell()->minimalSizeForOptimalDisplayFullSize() + constMargins();
}

}  // namespace Escher
