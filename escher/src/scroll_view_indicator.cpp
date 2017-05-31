#include <escher/scroll_view_indicator.h>
extern "C" {
#include <assert.h>
}

ScrollViewIndicator::ScrollViewIndicator(ScrollViewIndicator::Direction direction, KDColor indicatorColor, KDColor backgroundColor, KDCoordinate margin) :
  View(),
  m_direction(direction),
  m_start(0),
  m_end(0),
  m_indicatorColor(indicatorColor),
  m_backgroundColor(backgroundColor),
  m_margin(margin)
{
}

void ScrollViewIndicator::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect frame = KDRectZero;
  if (m_direction == Direction::Horizontal) {
    frame = KDRect(m_margin, (m_frame.height() - k_indicatorThickness)/2,
        m_frame.width() - 2*m_margin, k_indicatorThickness);
  } else {
    assert(m_direction == Direction::Vertical);
    frame = KDRect((m_frame.width() - k_indicatorThickness)/2, m_margin,
        k_indicatorThickness, m_frame.height() - 2*m_margin);
  }
  ctx->fillRect(frame, m_backgroundColor);
  KDRect indicatorFrame = KDRectZero;
  if (m_direction == Direction::Horizontal) {
    KDCoordinate indicatorWidth = m_frame.width() - 2*m_margin;
    indicatorFrame = KDRect(m_margin+m_start*indicatorWidth, (m_frame.height() - k_indicatorThickness)/2,
        (m_end-m_start)*indicatorWidth, k_indicatorThickness);
  } else {
    assert(m_direction == Direction::Vertical);
    KDCoordinate indicatorHeight = m_frame.height() - 2*m_margin;
    indicatorFrame = KDRect((m_frame.width() - k_indicatorThickness)/2, m_margin+m_start*indicatorHeight,
        k_indicatorThickness, (m_end-m_start)*indicatorHeight);
  }
  ctx->fillRect(indicatorFrame, m_indicatorColor);
}

float ScrollViewIndicator::start() const {
  return m_start;
}

void ScrollViewIndicator::setStart(float start) {
  if (m_start != start) {
    m_start = start;
    markRectAsDirty(bounds());
  }
}

float ScrollViewIndicator::end() const {
  return m_end;
}

void ScrollViewIndicator::setEnd(float end) {
  if (m_end != end) {
    m_end = end;
    markRectAsDirty(bounds());
  }
}

KDRect ScrollViewIndicator::frame() {
  return m_frame;
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
