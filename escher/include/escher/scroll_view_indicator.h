#ifndef ESCHER_SCROLL_VIEW_INDICATOR_H
#define ESCHER_SCROLL_VIEW_INDICATOR_H

#include <escher/metric.h>
#include <escher/palette.h>
#include <escher/view.h>

namespace Escher {

class ScrollViewIndicator : public View {
 protected:
  constexpr static KDColor k_color = Palette::GrayDark;
};

class ScrollViewBar : public ScrollViewIndicator {
 public:
  ScrollViewBar();
  bool update(KDCoordinate totalContentLength, KDCoordinate contentOffset,
              KDCoordinate visibleContentLength);
  bool visible() const { return 0.0f < m_offset || m_visibleLength < 1.0f; }

 protected:
  constexpr static KDCoordinate k_indicatorThickness =
      Metric::ScrollViewBarThickness;
  constexpr static KDColor k_trackColor = Palette::GrayMiddle;
#if ESCHER_VIEW_LOGGING
  const char *className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
  float m_offset;
  float m_visibleLength;
};

class ScrollViewHorizontalBar : public ScrollViewBar {
 public:
  void drawRect(KDContext *ctx, KDRect rect) const override;

 private:
  constexpr static KDCoordinate k_leftMargin = Metric::CommonLeftMargin;
  constexpr static KDCoordinate k_rightMargin = Metric::CommonRightMargin;
  KDCoordinate totalLength() const {
    return bounds().width() - k_leftMargin - k_rightMargin;
  }
};

class ScrollViewVerticalBar : public ScrollViewBar {
 public:
  ScrollViewVerticalBar();
  void drawRect(KDContext *ctx, KDRect rect) const override;
  void setMargins(KDCoordinate top, KDCoordinate bottom);
  void setTopMargin(KDCoordinate top) { setMargins(top, m_bottomMargin); }

 private:
  KDCoordinate totalLength() const {
    return bounds().height() - m_topMargin - m_bottomMargin;
  }
  KDCoordinate m_topMargin;
  KDCoordinate m_bottomMargin;
};

class ScrollViewArrow : public ScrollViewIndicator {
 public:
  ScrollViewArrow();
  bool update(bool visible);
  void setBackgroundColor(KDColor c) { m_backgroundColor = c; }
  void setFont(KDFont::Size font) { m_font = font; }
  void drawRect(KDContext *ctx, KDRect rect) const override;
  virtual const char *arrow() const = 0;

 private:
  KDColor m_backgroundColor;
  KDFont::Size m_font;
  bool m_visible;
};

class ScrollViewLeftArrow : public ScrollViewArrow {
 public:
  const char *arrow() const override { return "<"; }
};

class ScrollViewRightArrow : public ScrollViewArrow {
 public:
  const char *arrow() const override { return ">"; }
};

}  // namespace Escher
#endif
