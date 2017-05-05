#include <escher/scroll_view.h>

extern "C" {
#include <assert.h>
}

ScrollView::ScrollView(View * contentView, ScrollViewDelegate * delegate, KDCoordinate topMargin, KDCoordinate rightMargin,
  KDCoordinate bottomMargin, KDCoordinate leftMargin, bool showIndicators, bool colorBackground,
  KDColor backgroundColor, KDCoordinate indicatorThickness, KDColor indicatorColor,
  KDColor backgroundIndicatorColor, KDCoordinate indicatorMargin) :
  View(),
  m_topMargin(topMargin),
  m_delegate(delegate),
  m_contentView(contentView),
  m_verticalScrollIndicator(ScrollViewIndicator::Direction::Vertical, indicatorColor, backgroundIndicatorColor, indicatorMargin),
  m_horizontalScrollIndicator(ScrollViewIndicator::Direction::Horizontal, indicatorColor, backgroundIndicatorColor, indicatorMargin),
  m_rightMargin(rightMargin),
  m_bottomMargin(bottomMargin),
  m_leftMargin(leftMargin),
  m_indicatorThickness(indicatorThickness),
  m_showIndicators(showIndicators),
  m_colorBackground(colorBackground),
  m_backgroundColor(backgroundColor)
{
  assert(m_delegate != nullptr);
}

bool ScrollView::hasVerticalIndicator() const {
  if (m_showIndicators) {
    return m_verticalScrollIndicator.end() < 1 || m_verticalScrollIndicator.start() > 0;
  }
  return false;
}

bool ScrollView::hasHorizontalIndicator() const {
  if (m_showIndicators) {
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
  if (!m_colorBackground) {
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
  if (m_showIndicators) {
    float contentHeight = m_contentView->bounds().height()+m_topMargin+m_bottomMargin;
    float verticalStart = contentOffset().y();
    float verticalEnd = contentOffset().y() + m_frame.height();
    m_verticalScrollIndicator.setStart(verticalStart/contentHeight);
    m_verticalScrollIndicator.setEnd(verticalEnd/contentHeight);
    float contentWidth = m_contentView->bounds().width()+m_leftMargin+m_rightMargin;
    float horizontalStart = contentOffset().x();
    float horizontalEnd = contentOffset().x() + m_frame.width();
    m_horizontalScrollIndicator.setStart(horizontalStart/contentWidth);
    m_horizontalScrollIndicator.setEnd(horizontalEnd/contentWidth);
  }
}

void ScrollView::setContentOffset(KDPoint offset) {
  if (m_delegate->setOffset(offset)) {
    layoutSubviews();
  }
}

KDPoint ScrollView::contentOffset() const {
  return m_delegate->offset();
}

KDCoordinate ScrollView::topMargin() const {
  return m_topMargin;
}

KDCoordinate ScrollView::leftMargin() const {
  return m_leftMargin;
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
