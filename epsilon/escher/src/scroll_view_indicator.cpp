#include <escher/scroll_view_indicator.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Escher {

ScrollViewBar::ScrollViewBar()
    : ScrollViewIndicator(), m_offset(0), m_visibleLength(0) {}

bool ScrollViewBar::update(KDCoordinate totalContentLength,
                           KDCoordinate contentOffset,
                           KDCoordinate visibleContentLength) {
  float offset = contentOffset;
  float visibleLength = visibleContentLength;
  offset = offset / totalContentLength;
  visibleLength = visibleLength / totalContentLength;
  if (m_offset != offset || m_visibleLength != visibleLength) {
    m_offset = offset;
    m_visibleLength = visibleLength;
    markWholeFrameAsDirty();
  }
  return visible();
}

void ScrollViewHorizontalBar::drawRect(KDContext* ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  KDCoordinate y = (bounds().height() - k_indicatorThickness) / 2;
  KDCoordinate height = k_indicatorThickness;
  ctx->fillRect(KDRect(k_leftMargin, y, totalLength(), height), k_trackColor);

  /* Round both ends of the thumb, so that it reaches the end of the track when
   * scrolled to the end, instead of truncating its length. */
  KDCoordinate x = int(std::round(m_offset * totalLength()));
  KDCoordinate width =
      std::min(int(std::round((m_offset + m_visibleLength) * totalLength())),
               int(totalLength())) -
      x;
  ctx->fillRect(KDRect(k_leftMargin + x, y, width, height), k_color);
}

ScrollViewVerticalBar::ScrollViewVerticalBar()
    : m_margins(Metric::CommonMargins.vertical()) {}

void ScrollViewVerticalBar::drawRect(KDContext* ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  KDCoordinate x = (bounds().width() - k_indicatorThickness) / 2;
  KDCoordinate width = k_indicatorThickness;
  ctx->fillRect(KDRect(x, m_margins.top(), width, totalLength()), k_trackColor);

  /* Round both ends of the thumb, so that it reaches the end of the track when
   * scrolled to the end, instead of truncating its length. */
  KDCoordinate y = int(std::round(m_offset * totalLength()));
  KDCoordinate height =
      std::min(int(std::round((m_offset + m_visibleLength) * totalLength())),
               int(totalLength())) -
      y;
  ctx->fillRect(KDRect(x, m_margins.top() + y, width, height), k_color);
}

ScrollViewArrow::ScrollViewArrow()
    : m_font(KDFont::Size::Large), m_visible(false) {}

bool ScrollViewArrow::update(bool visible) {
  if (m_visible != visible) {
    markWholeFrameAsDirty();
    m_visible = visible;
  }
  return visible;
}

void ScrollViewArrow::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->alignAndDrawString(arrow(), KDPointZero, bounds().size(),
                          {.style = {.glyphColor = k_color,
                                     .backgroundColor = m_backgroundColor,
                                     .font = m_font},
                           .horizontalAlignment = KDGlyph::k_alignLeft,
                           .verticalAlignment = KDGlyph::k_alignCenter},
                          m_visible);
}

#if ESCHER_VIEW_LOGGING
const char* ScrollViewBar::className() const { return "ScrollViewBar"; }

void ScrollViewBar::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " offset=\"" << m_offset << "\"";
  os << " visibleLength=\"" << m_visibleLength << "\"";
}
#endif

}  // namespace Escher
