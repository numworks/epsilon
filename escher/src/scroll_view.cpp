#include <escher/scroll_view.h>
extern "C" {
#include <assert.h>
}

constexpr KDCoordinate k_indicatorThickness = 20;

ScrollView::ScrollView(View * contentView) :
  View(),
  m_offset(KDPointZero),
  m_contentView(contentView),
  m_verticalScrollIndicator(ScrollViewIndicator(ScrollViewIndicator::Direction::Vertical))
{
  //setSubview(m_contentView, 0);
  //setSubview(&m_verticalScrollIndicator, 1);
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

void ScrollView::layoutSubviews() {
  // Layout indicators
  KDRect verticalIndicatorFrame = KDRect(
      m_frame.width() - k_indicatorThickness, 0,
      k_indicatorThickness, m_frame.height()
   );
  m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);

  // Layout contentView
  // We're only re-positionning the contentView, not modifying its size.
  KDRect contentFrame = KDRect(m_offset.opposite(), m_contentView->bounds().size());
  m_contentView->setFrame(contentFrame);
}

void ScrollView::setContentOffset(KDPoint offset) {
  m_offset = offset;

  float contentHeight = m_contentView->bounds().height();
  float start = offset.y();
  float end = offset.y() + m_frame.height();

  m_verticalScrollIndicator.setStart(start/contentHeight);
  m_verticalScrollIndicator.setEnd(end/contentHeight);

  layoutSubviews();
}

KDCoordinate ScrollView::maxContentWidth() {
  return m_frame.width() - k_indicatorThickness;
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
