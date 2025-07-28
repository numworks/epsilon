#ifndef SHARED_CURVE_VIEW_CURSOR_H
#define SHARED_CURVE_VIEW_CURSOR_H

namespace Shared {

class CurveViewCursor {
 public:
  CurveViewCursor(double maxValue = k_defaultMaxValue);
  double t() const { return m_t; }
  double x() const { return m_x; }
  double y() const { return m_y; }
  void moveTo(double t, double x, double y);

 private:
  constexpr static double k_defaultMaxValue = 1E+8;
  double clipped(double f, bool canBeInfinite);

  double m_t;
  double m_x;
  double m_y;
  const double m_maxValue;
};

}  // namespace Shared

#endif
