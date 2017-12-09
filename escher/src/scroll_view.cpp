#include <escher/scroll_view.h>
#include <escher/palette.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

ScrollView::ScrollView(View * contentView, ScrollViewDataSource * dataSource) :
  View(),
  m_contentView(contentView),
  m_dataSource(dataSource),
  m_verticalScrollIndicator(ScrollViewIndicator::Direction::Vertical),
  m_horizontalScrollIndicator(ScrollViewIndicator::Direction::Horizontal),
  m_topMargin(0),
  m_rightMargin(0),
  m_bottomMargin(0),
  m_leftMargin(0),
  m_indicatorThickness(20),
  m_showsIndicators(true),
  m_colorsBackground(true),
  m_backgroundColor(Palette::WallScreen)
{
  assert(m_dataSource != nullptr);
}

void ScrollView::setCommonMargins() {
  setTopMargin(Metric::CommonTopMargin);
  setRightMargin(Metric::CommonRightMargin);
  setBottomMargin(Metric::CommonBottomMargin);
  setLeftMargin(Metric::CommonLeftMargin);
}

bool ScrollView::hasVerticalIndicator() const {
  if (m_showsIndicators) {
    return m_verticalScrollIndicator.end() < 1 || m_verticalScrollIndicator.start() > 0;
  }
  return false;
}

bool ScrollView::hasHorizontalIndicator() const {
  if (m_showsIndicators) {
    return m_horizontalScrollIndicator.end() < 1 || m_horizontalScrollIndicator.start() > 0;
  }
  return false;
}

int ScrollView::numberOfSubviews() const {
  return 1 + hasVerticalIndicator() + hasHorizontalIndicator();
}

View * ScrollView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_contentView;
    case 1:
      return hasHorizontalIndicator() ? &m_horizontalScrollIndicator : &m_verticalScrollIndicator;
    case 2:
      return &m_verticalScrollIndicator;
    }
  return nullptr;
}

void ScrollView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_colorsBackground) {
    return;
  }
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate offsetX = contentOffset().x();
  KDCoordinate offsetY = contentOffset().y();
  KDCoordinate contentHeight = m_contentView->bounds().height();
  KDCoordinate contentWidth = m_contentView->bounds().width();
  ctx->fillRect(KDRect(0, 0, width, m_topMargin-offsetY), m_backgroundColor);
  ctx->fillRect(KDRect(0, contentHeight+m_topMargin-offsetY, width, height - contentHeight - m_topMargin + offsetY), m_backgroundColor);
  ctx->fillRect(KDRect(0, 0, m_leftMargin-offsetX, height), m_backgroundColor);
  ctx->fillRect(KDRect(contentWidth + m_leftMargin - offsetX, 0, width - contentWidth - m_leftMargin + offsetX, height), m_backgroundColor);
}

void ScrollView::scrollToContentPoint(KDPoint p, bool allowOverscroll) {
  if (!allowOverscroll && !m_contentView->bounds().contains(p)) {
    return;
  }
  KDCoordinate offsetX = 0;
  KDCoordinate offsetY = 0;
  KDRect visibleRect = visibleContentRect();
  if (visibleRect.left() > p.x()) {
    offsetX = p.x() - visibleRect.left();
  }
  if (visibleRect.right() < p.x()) {
    offsetX = p.x() - visibleRect.right();
  }
  if (visibleRect.top() > p.y()) {
    offsetY = p.y() - visibleRect.top();
  }
  if (visibleRect.bottom() < p.y()) {
    offsetY = p.y() - visibleRect.bottom();
  }
  if (offsetX != 0 || offsetY != 0) {
    setContentOffset(contentOffset().translatedBy(KDPoint(offsetX, offsetY)));
  }

  /* Handle cases when the size of the view has decreased. */
  KDCoordinate contentOffsetX = contentOffset().x();
  KDCoordinate contentOffsetY = contentOffset().y();
  if (maxContentHeightDisplayableWithoutScrolling() > contentSize().height()-contentOffsetY) {
    contentOffsetY = contentSize().height() > maxContentHeightDisplayableWithoutScrolling() ? contentSize().height()-maxContentHeightDisplayableWithoutScrolling() : 0;
  }
  if (maxContentWidthDisplayableWithoutScrolling() > contentSize().width()-contentOffsetX) {
    contentOffsetX = contentSize().width() > maxContentWidthDisplayableWithoutScrolling() ? contentSize().width()-maxContentWidthDisplayableWithoutScrolling() : 0;
  }
  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
}

void ScrollView::scrollToContentRect(KDRect rect, bool allowOverscroll) {
  scrollToContentPoint(rect.topLeft(), allowOverscroll);
  scrollToContentPoint(rect.bottomRight(), allowOverscroll);
}

KDRect ScrollView::visibleContentRect() {
  return KDRect(
    contentOffset().x(),
    contentOffset().y(),
    m_frame.width() - m_leftMargin - m_rightMargin,
    m_frame.height() - m_topMargin - m_bottomMargin
  );
}

void ScrollView::layoutSubviews() {
  // Layout contentView
  // We're only re-positionning the contentView, not modifying its size.
  KDPoint absoluteOffset = contentOffset().opposite().translatedBy(KDPoint(m_leftMargin, m_topMargin));
  KDRect contentFrame = KDRect(absoluteOffset, m_contentView->bounds().size());
  m_contentView->setFrame(contentFrame);

  // We recompute the size of the scroll indicator
  updateScrollIndicator();

  // Layout indicators
  /* If the two indicators are visible, we leave an empty rectangle in the right
   * bottom corner. Otherwise, the only indicator uses all the height/width. */
  if (hasHorizontalIndicator() && hasVerticalIndicator()) {
      KDRect verticalIndicatorFrame = KDRect(
      m_frame.width() - m_indicatorThickness, 0,
      m_indicatorThickness, m_frame.height() - m_indicatorThickness
    );
    m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);
    KDRect horizontalIndicatorFrame = KDRect(
      0, m_frame.height() - m_indicatorThickness,
      m_frame.width() - m_indicatorThickness, m_indicatorThickness
    );
  m_horizontalScrollIndicator.setFrame(horizontalIndicatorFrame);
  } else {
    if (hasVerticalIndicator()) {
      KDRect verticalIndicatorFrame = KDRect(
      m_frame.width() - m_indicatorThickness, 0,
      m_indicatorThickness, m_frame.height()
      );
      m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);
    }
    if (hasHorizontalIndicator()) {
      KDRect horizontalIndicatorFrame = KDRect(
      0, m_frame.height() - m_indicatorThickness,
      m_frame.width(), m_indicatorThickness
      );
      m_horizontalScrollIndicator.setFrame(horizontalIndicatorFrame);
    }
  }
}

void ScrollView::updateScrollIndicator() {
  if (!m_showsIndicators) {
    return;
  }
  float contentHeight = m_contentView->bounds().height()+m_topMargin+m_bottomMargin;
  bool hadVerticalIndicator = hasVerticalIndicator();
  float verticalStart = contentOffset().y();
  float verticalEnd = contentOffset().y() + m_frame.height();
  m_verticalScrollIndicator.setStart(verticalStart/contentHeight);
  m_verticalScrollIndicator.setEnd(verticalEnd/contentHeight);
  if (hadVerticalIndicator && !hasVerticalIndicator()) {
    markRectAsDirty(m_verticalScrollIndicator.frame());
  }
  float contentWidth = m_contentView->bounds().width()+m_leftMargin+m_rightMargin;
  bool hadHorizontalIndicator = hasHorizontalIndicator();
  float horizontalStart = contentOffset().x();
  float horizontalEnd = contentOffset().x() + m_frame.width();
  m_horizontalScrollIndicator.setStart(horizontalStart/contentWidth);
  m_horizontalScrollIndicator.setEnd(horizontalEnd/contentWidth);
  if (hadHorizontalIndicator && !hasHorizontalIndicator()) {
    markRectAsDirty(m_horizontalScrollIndicator.frame());
  }
}

void ScrollView::setContentOffset(KDPoint offset, bool forceRelayout) {
  if (m_dataSource->setOffset(offset) || forceRelayout) {
    layoutSubviews();
  }
}

KDCoordinate ScrollView::maxContentWidthDisplayableWithoutScrolling() {
  return m_frame.width() - m_leftMargin - m_rightMargin;
}

KDCoordinate ScrollView::maxContentHeightDisplayableWithoutScrolling() {
  return m_frame.height() - m_topMargin - m_bottomMargin;
}

#if ESCHER_VIEW_LOGGING
const char * ScrollView::className() const {
  return "ScrollView";
}

void ScrollView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << (int)contentOffset().x << "," << (int)contentOffset().y << "\"";
}
#endif
