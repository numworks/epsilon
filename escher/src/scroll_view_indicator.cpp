#include <escher/scroll_view_indicator.h>
extern "C" {
#include <assert.h>
}

constexpr KDColor k_backgroundColor = KDColorBlack;
constexpr KDColor k_indicatorColor = KDColorRed;

ScrollViewIndicator::ScrollViewIndicator(ScrollViewIndicator::Direction direction) :
  View(),
  m_direction(direction),
  m_start(0),
  m_end(0)
{
}

void ScrollViewIndicator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  KDRect indicatorFrame = KDRectZero;
  if (m_direction == Direction::Horizontal) {
    indicatorFrame = KDRect(
        m_start*m_frame.width(), 0,
        (m_end-m_start)*m_frame.width(), m_frame.height()
        );
  } else {
    assert(m_direction == Direction::Vertical);
    indicatorFrame = KDRect(
        0, m_start*m_frame.height(),
        m_frame.width(), (m_end-m_start)*m_frame.height()
        );
  }
  ctx->fillRect(indicatorFrame, k_indicatorColor);
}

float ScrollViewIndicator::start() const {
  return m_start;
}

void ScrollViewIndicator::setStart(float start) {
  m_start = start;
  markRectAsDirty(bounds());
}

float ScrollViewIndicator::end() const {
  return m_end;
}

void ScrollViewIndicator::setEnd(float end) {
  m_end = end;
  markRectAsDirty(bounds());
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
