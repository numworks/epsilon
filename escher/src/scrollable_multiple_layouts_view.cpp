#include <assert.h>
#include <escher/scrollable_multiple_layouts_view.h>

#include <algorithm>

using namespace Poincare;

namespace Escher {

AbstractScrollableMultipleLayoutsView::ContentCell::ContentCell(
    float horizontalAlignment, KDFont::Size font)
    : m_rightLayoutView({.style = {.font = font}}),
      m_approximateSign(
          k_defaultApproximateMessage,
          {.style = {.glyphColor = Palette::GrayVeryDark, .font = font},
           .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_centeredLayoutView({.style = {.font = font}}),
      m_selectedSubviewPosition(SubviewPosition::Center),
      m_displayCenter(true),
      m_displayableCenter(true),
      m_rightIsStrictlyEqual(false),
      m_horizontalAlignment(horizontalAlignment) {}

KDColor AbstractScrollableMultipleLayoutsView::ContentCell::backgroundColor()
    const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
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
  KDColor defaultColor = backgroundColor();
  bool highlight = isHighlighted();
  KDColor color =
      highlight && m_selectedSubviewPosition == SubviewPosition::Center
          ? Palette::Select
          : defaultColor;
  m_centeredLayoutView.setBackgroundColor(color);
  color = highlight && m_selectedSubviewPosition == SubviewPosition::Right
              ? Palette::Select
              : defaultColor;
  m_rightLayoutView.setBackgroundColor(color);
  m_approximateSign.setBackgroundColor(defaultColor);
  if (leftLayoutView()) {
    color = highlight && m_selectedSubviewPosition == SubviewPosition::Left
                ? Palette::Select
                : defaultColor;
    leftLayoutView()->setBackgroundColor(color);
  }
}

void AbstractScrollableMultipleLayoutsView::ContentCell::reloadTextColor() {
  m_rightLayoutView.setTextColor(displayCenter() ? Palette::GrayVeryDark
                                                 : KDColorBlack);
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

void AbstractScrollableMultipleLayoutsView::ContentCell::
    setExactAndApproximateAreStriclyEqual(bool isEqual) {
  m_rightIsStrictlyEqual = isEqual;
  approximateSign()->setMessage(isEqual ? I18n::Message::Equal
                                        : I18n::Message::AlmostEqual);
  reloadTextColor();
}

Layout AbstractScrollableMultipleLayoutsView::ContentCell::layoutAtPosition(
    SubviewPosition position) const {
  if (position == SubviewPosition::Center) {
    return m_centeredLayoutView.layout();
  } else if (position == SubviewPosition::Right) {
    return m_rightLayoutView.layout();
  }
  assert(position == SubviewPosition::Left);
  assert(leftLayoutView());
  return leftLayoutView()->layout();
}

int AbstractScrollableMultipleLayoutsView::ContentCell::numberOfSubviews()
    const {
  int nbOfSubviews = 1;
  if (displayCenter()) {
    nbOfSubviews += 2;
  }
  if (leftLayoutView()) {
    nbOfSubviews += 1;
  }
  return nbOfSubviews;
}

KDCoordinate AbstractScrollableMultipleLayoutsView::ContentCell::baseline(
    KDCoordinate* leftBaseline, KDCoordinate* centerBaseline,
    KDCoordinate* rightBaseline) const {
  // Left view
  KDCoordinate leftViewBaseline =
      (leftLayoutView() && !leftLayoutView()->layout().isUninitialized())
          ? leftLayoutView()->layout().baseline(leftLayoutView()->font())
          : 0;
  if (leftBaseline != nullptr) {
    *leftBaseline = leftViewBaseline;
  }

  // Center view
  KDCoordinate centerViewBaseline =
      displayCenter() ? m_centeredLayoutView.layout().baseline(font()) : 0;
  if (centerBaseline != nullptr) {
    *centerBaseline = centerViewBaseline;
  }

  // Right view
  KDCoordinate rightViewBaseline =
      m_rightLayoutView.layout().isUninitialized()
          ? 0
          : m_rightLayoutView.layout().baseline(font());
  if (rightBaseline != nullptr) {
    *rightBaseline = rightViewBaseline;
  }

  return std::max({leftViewBaseline, centerViewBaseline, rightViewBaseline});
}

void AbstractScrollableMultipleLayoutsView::ContentCell::subviewFrames(
    KDRect* leftFrame, KDRect* centerFrame, KDRect* approximateSignFrame,
    KDRect* rightFrame) {
  // Subviews sizes
  KDSize leftSize = leftLayoutView()
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
  if (leftLayoutView() && !leftLayoutView()->layout().isUninitialized()) {
    assert(leftFrame != nullptr);
    *leftFrame = KDRect(currentWidth, viewBaseline - leftBaseline, leftSize);
    currentWidth += leftSize.width() +
                    AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
  }

  // Layout center layout
  if (displayCenter()) {
    assert(centerFrame != nullptr && approximateSignFrame != nullptr);
    KDSize approximateSignSize =
        m_approximateSign.minimalSizeForOptimalDisplay();
    *centerFrame =
        KDRect(currentWidth, viewBaseline - centerBaseline, centerSize);
    currentWidth += AbstractScrollableMultipleLayoutsView::k_horizontalMargin +
                    centerSize.width();
    *approximateSignFrame =
        displayApproximateSign()
            ? KDRect(currentWidth,
                     viewBaseline - approximateSignSize.height() / 2,
                     approximateSignSize)
            : KDRectZero;
    currentWidth += AbstractScrollableMultipleLayoutsView::k_horizontalMargin +
                    approximateSignSize.width();
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
    if (leftLayoutView()) {
      *leftFrame = leftFrame->translatedBy(offset);
    }
    if (displayCenter()) {
      *centerFrame = centerFrame->translatedBy(offset);
      *approximateSignFrame = approximateSignFrame->translatedBy(offset);
    }
  }
}

KDSize AbstractScrollableMultipleLayoutsView::ContentCell::
    privateMinimalSizeForOptimalDisplay(bool forceFullDisplay) const {
  KDCoordinate width = 0;
  KDCoordinate height = 0;

  KDSize leftSize = KDSizeZero;
  if (leftLayoutView() && !leftLayoutView()->layout().isUninitialized()) {
    leftSize = leftLayoutView()->minimalSizeForOptimalDisplay();
    width += leftSize.width() +
             AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
    height = std::max(height, leftSize.height());
  }

  KDSize centerSize = KDSizeZero;
  if (displayCenter() || (forceFullDisplay && displayableCenter())) {
    centerSize = m_centeredLayoutView.minimalSizeForOptimalDisplay();
    width += centerSize.width() +
             2 * AbstractScrollableMultipleLayoutsView::k_horizontalMargin +
             m_approximateSign.minimalSizeForOptimalDisplay().width();
    height = std::max(height, centerSize.height());
  }

  KDSize rightSize = KDSizeZero;
  if (!m_rightLayoutView.layout().isUninitialized()) {
    rightSize = m_rightLayoutView.minimalSizeForOptimalDisplay();
    width += rightSize.width();
  }

  height = std::max(height, rightSize.height());

  return KDSize(width, height);
}

View* AbstractScrollableMultipleLayoutsView::ContentCell::subviewAtIndex(
    int index) {
  bool leftIsVisible = leftLayoutView() != nullptr;
  if (leftIsVisible && index == 0) {
    return leftLayoutView();
  }
  View* views[3] = {&m_rightLayoutView, &m_approximateSign,
                    &m_centeredLayoutView};
  return views[index - leftIsVisible];
}

void AbstractScrollableMultipleLayoutsView::ContentCell::layoutSubviews(
    bool force) {
  if (bounds().width() <= 0 || bounds().height() <= 0) {
    /* TODO Make this behaviour in a non-virtual layoutSublviews, and all layout
     * subviews should become privateLayoutSubviews */
    return;
  }
  KDRect leftFrame = KDRectZero;
  KDRect centerFrame = KDRectZero;
  KDRect approximateSignFrame = KDRectZero;
  KDRect rightFrame = KDRectZero;
  subviewFrames(&leftFrame, &centerFrame, &approximateSignFrame, &rightFrame);
  if (leftLayoutView() != nullptr) {
    setChildFrame(leftLayoutView(), leftFrame, force);
  }
  if (centeredLayoutView() != nullptr) {
    setChildFrame(centeredLayoutView(), centerFrame, force);
  }
  setChildFrame(&m_approximateSign, approximateSignFrame, force);
  if (rightLayoutView() != nullptr) {
    setChildFrame(rightLayoutView(), rightFrame, force);
  }
}

AbstractScrollableMultipleLayoutsView::AbstractScrollableMultipleLayoutsView(
    Responder* parentResponder, View* contentCell)
    : ScrollableView(parentResponder, contentCell, this) {}

void AbstractScrollableMultipleLayoutsView::setLayouts(
    Layout formulaLayout, Layout exactLayout, Layout approximateLayout) {
  bool updateRightLayout =
      contentCell()->rightLayoutView()->setLayout(approximateLayout);
  bool updateCenterLayout =
      contentCell()->centeredLayoutView()->setLayout(exactLayout);
  bool updateLeftLayout = false;
  if (contentCell()->leftLayoutView()) {
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
  if (selectedSubviewPosition() == SubviewPosition::Right) {
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
    if (contentCell()->leftLayoutView()) {
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
                                   ->approximateSign()
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
      return true;
    }
    if ((event == Ion::Events::Right &&
         selectedSubviewPosition() == SubviewPosition::Center &&
         rightLayoutIsVisible) ||
        (event == Ion::Events::Right &&
         selectedSubviewPosition() == SubviewPosition::Left &&
         rightLayoutIsVisible)) {
      setSelectedSubviewPosition(SubviewPosition::Right);
      return true;
    }
  }
  return ScrollableView::handleEvent(event);
}

KDSize ScrollableTwoLayoutsView::minimalSizeForOptimalDisplayFullSize() const {
  return contentCell()->minimalSizeForOptimalDisplayFullSize() + constMargins();
}

}  // namespace Escher
