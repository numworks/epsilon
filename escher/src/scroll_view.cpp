#include <escher/scroll_view.h>
#include <escher/palette.h>

extern "C" {
#include <assert.h>
}

constexpr KDCoordinate ScrollView::k_indicatorThickness;

ScrollView::ScrollView(View * contentView, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  View(),
  m_offset(KDPointZero),
  m_contentView(contentView),
  m_verticalScrollIndicator(ScrollViewIndicator(ScrollViewIndicator::Direction::Vertical)),
  m_topMargin(topMargin),
  m_rightMargin(rightMargin),
  m_bottomMargin(bottomMargin),
  m_leftMargin(leftMargin)
{
}

int ScrollView::numberOfSubviews() const {
  return 2;
}

View * ScrollView::subviewAtIndex(int index) {
  switch(index) {
    case 0:
      return m_contentView;
    case 1:
      return &m_verticalScrollIndicator;
  }
  assert(false);
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
  // Layout indicators
  KDRect verticalIndicatorFrame = KDRect(
      m_frame.width() - k_indicatorThickness, 0,
      k_indicatorThickness, m_frame.height()
   );
  m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);

  // Layout contentView
  // We're only re-positionning the contentView, not modifying its size.
  KDPoint absoluteOffset = m_offset.opposite().translatedBy(KDPoint(m_leftMargin, m_topMargin));
  KDRect contentFrame = KDRect(absoluteOffset, m_contentView->bounds().size());
  m_contentView->setFrame(contentFrame);

  // We recompute the size of the scroll indicator
  updateScrollIndicator();
}

void ScrollView::updateScrollIndicator() {
  float contentHeight = m_contentView->bounds().height()+m_topMargin+m_bottomMargin;
  float start = m_offset.y();
  float end = m_offset.y() + m_frame.height();

  m_verticalScrollIndicator.setStart(start/contentHeight);
  m_verticalScrollIndicator.setEnd(end/contentHeight);
}

void ScrollView::setContentOffset(KDPoint offset) {
  m_offset = offset;
  layoutSubviews();
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
