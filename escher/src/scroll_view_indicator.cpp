#include <escher/scroll_view_indicator.h>
#include <escher/palette.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Escher {

ScrollViewIndicator::ScrollViewIndicator() :
  View(),
  m_color(Palette::GrayDark)
{
}

ScrollViewBar::ScrollViewBar() :
  ScrollViewIndicator(),
  m_offset(0),
  m_visibleLength(0),
  m_trackColor(Palette::GrayMiddle)
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

ScrollViewHorizontalBar::ScrollViewHorizontalBar() :
  m_leftMargin(Metric::CommonLeftMargin),
  m_rightMargin(Metric::CommonRightMargin)
{
}

void ScrollViewHorizontalBar::drawRect(KDContext * ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  ctx->fillRect(
    KDRect(
      m_leftMargin, (m_frame.height() - k_indicatorThickness)/2,
      totalLength(), k_indicatorThickness
    ),
    m_trackColor
  );
  ctx->fillRect(
    KDRect(
      m_leftMargin+m_offset*totalLength(), (m_frame.height() - k_indicatorThickness)/2,
      std::ceil(m_visibleLength*totalLength()), k_indicatorThickness
    ),
    m_color
  );
}

ScrollViewVerticalBar::ScrollViewVerticalBar() :
  m_topMargin(Metric::CommonTopMargin),
  m_bottomMargin(Metric::CommonBottomMargin)
{
}

void ScrollViewVerticalBar::setMargins(KDCoordinate top, KDCoordinate bottom) {
  m_topMargin = top;
  m_bottomMargin = bottom;
}

void ScrollViewVerticalBar::drawRect(KDContext * ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  ctx->fillRect(
    KDRect(
      (m_frame.width() - k_indicatorThickness)/2, m_topMargin,
      k_indicatorThickness, totalLength()
    ),
    m_trackColor
  );
  ctx->fillRect(
    KDRect(
      (m_frame.width() - k_indicatorThickness)/2, m_topMargin+m_offset*totalLength(),
      k_indicatorThickness, std::ceil(m_visibleLength*totalLength())
    ),
    m_color
  );
}

ScrollViewArrow::ScrollViewArrow(Side side) :
  m_visible(false),
  m_arrow(side),
  m_font(KDFont::Size::Large)
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
  char arrowString[2] = {m_arrow, 0}; // TODO Change when code points
  ctx->alignAndDrawString(arrowString, KDPointZero, m_frame.size(),
    (m_arrow == Top || m_arrow == Bottom) ? KDContext::k_alignCenter : KDContext::k_alignLeft,
    (m_arrow == Left || m_arrow == Right) ? KDContext::k_alignCenter : KDContext::k_alignTop,
    m_font, m_color, m_backgroundColor, m_visible);
}

#if ESCHER_VIEW_LOGGING
const char * ScrollViewBar::className() const {
  return "ScrollViewBar";
}

void ScrollViewBar::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << m_offset << "\"";
  os << " visibleLength=\"" << m_visibleLength << "\"";
}
#endif

}
