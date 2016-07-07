#ifndef ESCHER_SCROLL_VIEW_INDICATOR_H
#define ESCHER_SCROLL_VIEW_INDICATOR_H

#include <escher/childless_view.h>

class ScrollViewIndicator : public ChildlessView {
public:
  enum class Direction {
    Horizontal,
    Vertical
  };
  ScrollViewIndicator(Direction direction);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  void setStart(float start);
  void setEnd(float end);
protected:
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
private:
  Direction m_direction;
  float m_start;
  float m_end;
};

#endif
