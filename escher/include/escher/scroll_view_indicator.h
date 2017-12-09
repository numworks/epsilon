#ifndef ESCHER_SCROLL_VIEW_INDICATOR_H
#define ESCHER_SCROLL_VIEW_INDICATOR_H

#include <escher/view.h>

class ScrollViewIndicator final : public View {
public:
  enum class Direction {
    Horizontal,
    Vertical
  };
  ScrollViewIndicator(Direction direction);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  void setIndicatorColor(KDColor c) { m_indicatorColor = c; }
  KDColor indicatorColor() const { return m_indicatorColor; }
  void setBackgroundColor(KDColor c) { m_backgroundColor = c; }
  KDColor backgroundColor() const { return m_backgroundColor; }
  void setMargin(KDCoordinate m) { m_margin = m; }
  KDCoordinate margin() const { return m_margin; }

  float start() const { return m_start; }
  void setStart(float start);
  float end() const { return m_end; }
  void setEnd(float end);
  KDRect frame() { return m_frame; }
protected:
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
private:
  constexpr static KDCoordinate k_indicatorThickness = 4;
  Direction m_direction;
  float m_start;
  float m_end;
  KDColor m_indicatorColor;
  KDColor m_backgroundColor;
  KDCoordinate m_margin;
};

#endif
