#ifndef ESCHER_SCROLL_VIEW_INDICATOR_H
#define ESCHER_SCROLL_VIEW_INDICATOR_H

#include <escher/view.h>

class ScrollViewIndicator : public View {
public:
  ScrollViewIndicator();

  void setIndicatorColor(KDColor c) { m_indicatorColor = c; }
  KDColor indicatorColor() const { return m_indicatorColor; }
  void setBackgroundColor(KDColor c) { m_backgroundColor = c; }
  KDColor backgroundColor() const { return m_backgroundColor; }
  void setMargin(KDCoordinate m) { m_margin = m; }
  KDCoordinate margin() const { return m_margin; }

  void update(KDCoordinate totalContentLength, KDCoordinate contentOffset, KDCoordinate visibleContentLength);
  bool visible() const { return 0 < m_offset || m_visibleLength < 1; }
protected:
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
  constexpr static KDCoordinate k_indicatorThickness = 4;
  float m_offset;
  float m_visibleLength;
  KDColor m_indicatorColor;
  KDColor m_backgroundColor;
  KDCoordinate m_margin;
};

class ScrollViewHorizontalIndicator : public ScrollViewIndicator {
public:
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  KDCoordinate totalLength() const { return m_frame.width() - 2*m_margin; }
};

class ScrollViewVerticalIndicator : public ScrollViewIndicator {
public:
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  KDCoordinate totalLength() const { return m_frame.height() - 2*m_margin; }
};

#endif
