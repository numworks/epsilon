#ifndef SHARED_CURVE_VIEW_CURSOR_H
#define SHARED_CURVE_VIEW_CURSOR_H

namespace Shared {

class CurveViewCursor {
public:
  CurveViewCursor();
  double t() const { return m_t; }
  double x() const { return m_x; }
  double y() const { return m_y; }
  virtual void moveTo(double t, double x, double y);
protected:
  double m_t;
  double m_x;
  double m_y;
private:
  static double clipped(double f, bool canBeInfinite);
  constexpr static double k_maxFloat = 1E+8;
};

}

#endif
