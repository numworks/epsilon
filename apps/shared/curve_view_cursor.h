#ifndef SHARED_CURVE_VIEW_CURSOR_H
#define SHARED_CURVE_VIEW_CURSOR_H

namespace Shared {

class CurveViewCursor {
public:
  CurveViewCursor();
  float x();
  float y();
  void moveTo(float x, float y);
private:
  static float clipped(float f, bool canBeInfinite);
  constexpr static float k_maxFloat = 1E+8f;
  float m_x;
  float m_y;
};

}

#endif
