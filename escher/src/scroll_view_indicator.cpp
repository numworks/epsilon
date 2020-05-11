#include <escher/scroll_view_indicator.h>
#include <escher/metric.h>
#include <escher/palette.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

ScrollViewIndicator::ScrollViewIndicator() :
  View(),
  m_color(Palette::GreyDark),
  m_margin(Metric::CommonTopMargin)
{
}

ScrollViewBar::ScrollViewBar() :
  ScrollViewIndicator(),
  m_offset(0),
  m_visibleLength(0),
  m_trackColor(Palette::GreyMiddle)
{
}

bool ScrollViewBar::update(KDCoordinate totalContentLength, KDCoordinate contentOffset, KDCoordinate visibleContentLength) {
  float offset = contentOffset;
  float visibleLength = visibleContentLength;
  offset = offset / totalContentLength;
  visibleLength = visibleLength / totalContentLength;
  if (m_offset != offset || m_visibleLength != visibleLength) {
    m_offset = offset;
    m_visibleLength = visibleLength;
    markRectAsDirty(bounds());
  }
  return visible();
}

void ScrollViewHorizontalBar::drawRect(KDContext * ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  ctx->fillRect(
    KDRect(
      m_margin, (m_frame.height() - k_indicatorThickness)/2,
      totalLength(), k_indicatorThickness
    ),
    m_trackColor
  );
  ctx->fillRect(
    KDRect(
      m_margin+m_offset*totalLength(), (m_frame.height() - k_indicatorThickness)/2,
      std::ceil(m_visibleLength*totalLength()), k_indicatorThickness
    ),
    m_color
  );
}

void ScrollViewVerticalBar::drawRect(KDContext * ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  ctx->fillRect(
    KDRect(
      (m_frame.width() - k_indicatorThickness)/2, m_margin,
      k_indicatorThickness, totalLength()
    ),
    m_trackColor
  );
  ctx->fillRect(
    KDRect(
      (m_frame.width() - k_indicatorThickness)/2, m_margin+m_offset*totalLength(),
      k_indicatorThickness, std::ceil(m_visibleLength*totalLength())
    ),
    m_color
  );
}

ScrollViewArrow::ScrollViewArrow(Side side) :
  m_visible(false),
  m_arrow(side)
{
}

bool ScrollViewArrow::update(bool visible) {
  if (m_visible != visible) {
    markRectAsDirty(bounds());
  }
  m_visible = visible;
  return visible;
}

void ScrollViewArrow::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);
  KDSize arrowSize = KDFont::LargeFont->glyphSize();
  const KDPoint arrowAlign = KDPoint(
    (m_arrow == Top || m_arrow == Bottom) * (m_frame.width() - arrowSize.width()) / 2,
    (m_arrow == Left || m_arrow == Right) * (m_frame.height() - arrowSize.height()) / 2
  );
  char arrowString[2] = {m_arrow, 0}; // TODO Change when code points
  ctx->drawString(arrowString, arrowAlign, KDFont::LargeFont, m_color, m_backgroundColor, m_visible);
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
