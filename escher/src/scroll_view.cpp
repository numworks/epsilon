#include <escher/scroll_view.h>
#include <escher/palette.h>

extern "C" {
#include <assert.h>
}

constexpr KDCoordinate ScrollView::k_indicatorThickness;

ScrollView::ScrollView(View * contentView, KDCoordinate topMargin, KDCoordinate rightMargin,
  KDCoordinate bottomMargin, KDCoordinate leftMargin, bool showIndicators) :
  View(),
  m_offset(KDPointZero),
  m_contentView(contentView),
  m_verticalScrollIndicator(ScrollViewIndicator(ScrollViewIndicator::Direction::Vertical)),
  m_horizontalScrollIndicator(ScrollViewIndicator(ScrollViewIndicator::Direction::Horizontal)),
  m_topMargin(topMargin),
  m_rightMargin(rightMargin),
  m_bottomMargin(bottomMargin),
  m_leftMargin(leftMargin),
  m_showIndicators(showIndicators)
{
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
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, width, height), Palette::BackgroundColor);
  // Future optimization: avoid drawing behind the content view/
  //ctx->fillRect(KDRect(m_leftMargin, m_contentView->bounds().height()+m_topMargin,  width-m_leftMargin-m_rightMargin, height- m_contentView->bounds().height())-m_topMargin, Palette::BackgroundColor);
}


void ScrollView::layoutSubviews() {
  // Layout contentView
  // We're only re-positionning the contentView, not modifying its size.
  KDPoint absoluteOffset = m_offset.opposite().translatedBy(KDPoint(m_leftMargin, m_topMargin));
  KDRect contentFrame = KDRect(absoluteOffset, m_contentView->bounds().size());
  m_contentView->setFrame(contentFrame);

  // We recompute the size of the scroll indicator
  updateScrollIndicator();

  // Layout indicators
  /* If the two indicators are visible, we leave an empty rectangle in the right
   * bottom corner. Otherwise, the only indicator uses all the height/width. */
  if (hasHorizontalIndicator() && hasVerticalIndicator()) {
      KDRect verticalIndicatorFrame = KDRect(
      m_frame.width() - k_indicatorThickness, 0,
      k_indicatorThickness, m_frame.height() - k_indicatorThickness
    );
    m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);
    KDRect horizontalIndicatorFrame = KDRect(
      0, m_frame.height() - k_indicatorThickness,
      m_frame.width() - k_indicatorThickness, k_indicatorThickness
    );
  m_horizontalScrollIndicator.setFrame(horizontalIndicatorFrame);
  } else {
    if (hasVerticalIndicator()) {
      KDRect verticalIndicatorFrame = KDRect(
      m_frame.width() - k_indicatorThickness, 0,
      k_indicatorThickness, m_frame.height()
      );
      m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);
    }
    if (hasHorizontalIndicator()) {
      KDRect horizontalIndicatorFrame = KDRect(
      0, m_frame.height() - k_indicatorThickness,
      m_frame.width(), k_indicatorThickness
      );
      m_horizontalScrollIndicator.setFrame(horizontalIndicatorFrame);
    }
  }
}

void ScrollView::updateScrollIndicator() {
  if (m_showIndicators) {
    float contentHeight = m_contentView->bounds().height()+m_topMargin+m_bottomMargin;
    float verticalStart = m_offset.y();
    float verticalEnd = m_offset.y() + m_frame.height();
    m_verticalScrollIndicator.setStart(verticalStart/contentHeight);
    m_verticalScrollIndicator.setEnd(verticalEnd/contentHeight);
    float contentWidth = m_contentView->bounds().width()+m_leftMargin+m_rightMargin;
    float horizontalStart = m_offset.x();
    float horizontalEnd = m_offset.x() + m_frame.width();
    m_horizontalScrollIndicator.setStart(horizontalStart/contentWidth);
    m_horizontalScrollIndicator.setEnd(horizontalEnd/contentWidth);
  }
}

void ScrollView::setContentOffset(KDPoint offset) {
  if (offset != m_offset) {
    m_offset = offset;
    layoutSubviews();
  }
}

KDPoint ScrollView::contentOffset() {
  return m_offset;
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
  os << " offset=\"" << (int)m_offset.x << "," << (int)m_offset.y << "\"";
}
#endif
