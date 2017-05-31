#ifndef ESCHER_SCROLL_VIEW_INDICATOR_H
#define ESCHER_SCROLL_VIEW_INDICATOR_H

#include <escher/view.h>

class ScrollViewIndicator : public View {
public:
  enum class Direction {
    Horizontal,
    Vertical
  };
  ScrollViewIndicator(Direction direction, KDColor indicatorColor, KDColor backgroundColor, KDCoordinate margin);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  float start() const;
  void setStart(float start);
  float end() const;
  void setEnd(float end);
  KDRect frame();
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
