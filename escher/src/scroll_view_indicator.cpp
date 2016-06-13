#include <escher/scroll_view_indicator.h>
extern "C" {
#include <assert.h>
}

constexpr KDColor k_backgroundColor = 0x00;
constexpr KDColor k_indicatorColor = 0x30;

ScrollViewIndicator::ScrollViewIndicator(ScrollViewIndicator::Direction direction) :
  ChildlessView(),
  m_direction(direction),
  m_start(0),
  m_end(0)
{
}

void ScrollViewIndicator::drawRect(KDRect rect) const {
  KDFillRect(bounds(), k_backgroundColor);
  KDRect indicatorFrame;
  if (m_direction == Direction::Horizontal) {
  indicatorFrame.x = m_start*m_frame.width;
  indicatorFrame.y = 0;
  indicatorFrame.width = (m_end-m_start)*m_frame.width;
  indicatorFrame.height = m_frame.height;
  } else {
    assert(m_direction == Direction::Vertical);
    indicatorFrame.x = 0;
    indicatorFrame.y = m_start*m_frame.height;
    indicatorFrame.width = m_frame.width;
    indicatorFrame.height = (m_end-m_start)*m_frame.height;
  }
  KDFillRect(indicatorFrame, k_indicatorColor);
}

void ScrollViewIndicator::setStart(float start) {
  m_start = start;
  markAsNeedingRedraw();
}

void ScrollViewIndicator::setEnd(float end) {
  m_end = end;
  markAsNeedingRedraw();
}

#if ESCHER_VIEW_LOGGING
const char * ScrollViewIndicator::className() const {
  return "ScrollViewIndicator";
}

void ScrollViewIndicator::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " start=\"" << m_start << "\"";
  os << " end=\"" << m_end << "\"";
}
#endif
