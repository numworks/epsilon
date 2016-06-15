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
  setSubview(m_contentView, 0);
  setSubview(&m_verticalScrollIndicator, 1);
}

int ScrollView::numberOfSubviews() const {
  return 2;
}

View * ScrollView::subview(int index) {
  switch(index) {
    case 0:
      return m_contentView;
    case 1:
      return &m_verticalScrollIndicator;
  }
  assert(false);
  return nullptr;
}

void ScrollView::storeSubviewAtIndex(View * view, int index) {
  switch (index) {
    case 0:
      m_contentView = view;
      break;
    case 1:
      assert(view == &m_verticalScrollIndicator);
      break;
  }
}

void ScrollView::layoutSubviews() {
  // Layout indicators
  KDRect verticalIndicatorFrame;
  verticalIndicatorFrame.x = m_frame.width - k_indicatorThickness;
  verticalIndicatorFrame.y = 0;
  verticalIndicatorFrame.width = k_indicatorThickness;
  verticalIndicatorFrame.height = m_frame.height;
  m_verticalScrollIndicator.setFrame(verticalIndicatorFrame);

  // Layout contentView
  KDRect contentFrame;
  contentFrame.x = -m_offset.x;
  contentFrame.y = -m_offset.y;
  // We want contentFrame.rightX = verticalIndicatorFrame.x;
  // contentFrame.x + contentFrame.width = verticalIndicatorFrame.x;
  contentFrame.width = verticalIndicatorFrame.x - contentFrame.x;
  contentFrame.height = m_offset.y + m_frame.height;
  m_contentView->setFrame(contentFrame);
}

void ScrollView::setContentOffset(KDPoint offset) {
  m_offset = offset;

  float contentHeight = m_contentView->bounds().height;
  float start = offset.x;
  float end = offset.x + m_frame.height;

  m_verticalScrollIndicator.setStart(start/contentHeight);
  m_verticalScrollIndicator.setEnd(end/contentHeight);

  layoutSubviews();
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
