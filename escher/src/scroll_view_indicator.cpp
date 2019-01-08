#include <escher/scroll_view_indicator.h>
#include <escher/palette.h>
extern "C" {
#include <assert.h>
}

ScrollViewIndicator::ScrollViewIndicator() :
  View(),
  m_offset(0),
  m_visibleLength(0),
  m_indicatorColor(Palette::GreyDark),
  m_backgroundColor(Palette::GreyMiddle),
  m_margin(14)
{
}

void ScrollViewHorizontalIndicator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(
    KDRect(
      m_margin, (m_frame.height() - k_indicatorThickness)/2,
      totalLength(), k_indicatorThickness
    ),
    m_backgroundColor
  );
  ctx->fillRect(
    KDRect(
      m_margin+m_offset*totalLength(), (m_frame.height() - k_indicatorThickness)/2,
      m_visibleLength*totalLength(), k_indicatorThickness
    ),
    m_indicatorColor
  );
}

void ScrollViewVerticalIndicator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(
    KDRect(
      (m_frame.width() - k_indicatorThickness)/2, m_margin,
      k_indicatorThickness, totalLength()
    ),
    m_backgroundColor
  );
  ctx->fillRect(
    KDRect(
      (m_frame.width() - k_indicatorThickness)/2, m_margin+m_offset*totalLength(),
      k_indicatorThickness, m_visibleLength*totalLength()
    ),
    m_indicatorColor
  );
}

void ScrollViewIndicator::update(KDCoordinate totalContentLength, KDCoordinate contentOffset, KDCoordinate visibleContentLength) {
  float offset = contentOffset;
  float visibleLength = visibleContentLength;
  offset = offset / totalContentLength;
  visibleLength = visibleLength / totalContentLength;
  if (m_offset != offset || m_visibleLength != visibleLength) {
    m_offset = offset;
    m_visibleLength = visibleLength;
    markRectAsDirty(bounds());
  }
}

#if ESCHER_VIEW_LOGGING
const char * ScrollViewIndicator::className() const {
  return "ScrollViewIndicator";
}

void ScrollViewIndicator::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << m_offset << "\"";
  os << " visibleLength=\"" << m_visibleLength << "\"";
}
#endif
