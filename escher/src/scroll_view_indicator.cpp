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
    markRectAsDirty(bounds());
  }
  return visible();
}

void ScrollViewHorizontalBar::drawRect(KDContext *ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  KDCoordinate y = (bounds().height() - k_indicatorThickness) / 2;
  KDCoordinate height = k_indicatorThickness;
  ctx->fillRect(KDRect(k_leftMargin, y, totalLength(), height), k_trackColor);

  KDCoordinate x = int(std::round(m_offset * totalLength()));
  KDCoordinate width =
      std::min(int(m_visibleLength * totalLength()), totalLength() - x);
  ctx->fillRect(KDRect(k_leftMargin + x, y, width, height), k_color);
}

ScrollViewVerticalBar::ScrollViewVerticalBar()
    : m_topMargin(Metric::CommonTopMargin),
      m_bottomMargin(Metric::CommonBottomMargin) {}

void ScrollViewVerticalBar::setMargins(KDCoordinate top, KDCoordinate bottom) {
  m_topMargin = top;
  m_bottomMargin = bottom;
}

void ScrollViewVerticalBar::drawRect(KDContext *ctx, KDRect rect) const {
  if (!visible()) {
    return;
  }
  KDCoordinate x = (bounds().width() - k_indicatorThickness) / 2;
  KDCoordinate width = k_indicatorThickness;
  ctx->fillRect(KDRect(x, m_topMargin, width, totalLength()), k_trackColor);

  KDCoordinate y = int(std::round(m_offset * totalLength()));
  KDCoordinate height =
      std::min(int(m_visibleLength * totalLength()), totalLength() - y);
  ctx->fillRect(KDRect(x, m_topMargin + y, width, height), k_color);
}

ScrollViewArrow::ScrollViewArrow()
    : m_font(KDFont::Size::Large), m_visible(false) {}

bool ScrollViewArrow::update(bool visible) {
  if (m_visible != visible) {
    markRectAsDirty(bounds());
  }
  m_visible = visible;
  return visible;
}

void ScrollViewArrow::drawRect(KDContext *ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->alignAndDrawString(arrow(), KDPointZero, bounds().size(),
                          {{.glyphColor = k_color,
                            .backgroundColor = m_backgroundColor,
                            .font = m_font},
                           .horizontalAlignment = KDGlyph::k_alignLeft,
                           .verticalAlignment = KDGlyph::k_alignCenter},
                          m_visible);
}

#if ESCHER_VIEW_LOGGING
const char *ScrollViewBar::className() const { return "ScrollViewBar"; }

void ScrollViewBar::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << m_offset << "\"";
  os << " visibleLength=\"" << m_visibleLength << "\"";
}
#endif

}  // namespace Escher
