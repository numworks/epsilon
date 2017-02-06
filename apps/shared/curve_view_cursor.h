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
  float m_x;
  float m_y;
};

}

#endif
