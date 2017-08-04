#ifndef SHARED_CURVE_VIEW_CURSOR_H
#define SHARED_CURVE_VIEW_CURSOR_H

namespace Shared {

class CurveViewCursor {
public:
  CurveViewCursor();
  double x();
  double y();
  void moveTo(double x, double y);
private:
  static double clipped(double f, bool canBeInfinite);
  constexpr static double k_maxFloat = 1E+8;
  double m_x;
  double m_y;
};

}

#endif
