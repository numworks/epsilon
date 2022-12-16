#ifndef ESCHER_SCROLL_VIEW_INDICATOR_H
#define ESCHER_SCROLL_VIEW_INDICATOR_H

#include <escher/view.h>
#include <escher/metric.h>

namespace Escher {

class ScrollViewIndicator : public View {
public:
  ScrollViewIndicator();
protected:
  KDColor m_color;
};

class ScrollViewBar : public ScrollViewIndicator {
public:
  ScrollViewBar();
  bool update(KDCoordinate totalContentLength, KDCoordinate contentOffset, KDCoordinate visibleContentLength);
  bool visible() const { return 0.0f < m_offset || m_visibleLength < 1.0f; }
protected:
  constexpr static KDCoordinate k_indicatorThickness = Metric::ScrollViewBarThickness;
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
  float m_offset;
  float m_visibleLength;
  KDColor m_trackColor;
};

class ScrollViewHorizontalBar : public ScrollViewBar {
public:
  ScrollViewHorizontalBar();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  KDCoordinate totalLength() const { return m_frame.width() - m_leftMargin - m_rightMargin; }
  KDCoordinate m_leftMargin;
  KDCoordinate m_rightMargin;
};

class ScrollViewVerticalBar : public ScrollViewBar {
public:
  ScrollViewVerticalBar();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setMargins(KDCoordinate top, KDCoordinate bottom);
  void setTopMargin(KDCoordinate top) { setMargins(top, m_bottomMargin); }
private:
  KDCoordinate totalLength() const { return m_frame.height() - m_topMargin - m_bottomMargin; }
  KDCoordinate m_topMargin;
  KDCoordinate m_bottomMargin;
};

class ScrollViewArrow : public ScrollViewIndicator {
public:
  enum Side : char { //FIXME
    Top = 't',
    Right = '>',
    Bottom = 'b',
    Left = '<'
  };
  ScrollViewArrow(Side side);
  bool update(bool visible);
  void setBackgroundColor(KDColor c) { m_backgroundColor = c; }
  void setFont(KDFont::Size font) { m_font = font; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  bool m_visible;
  const char m_arrow;
  KDColor m_backgroundColor;
  KDFont::Size m_font;
};

}
#endif
