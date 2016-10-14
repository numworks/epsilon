#ifndef ESCHER_FLOAT_VIEW_H
#define ESCHER_FLOAT_VIEW_H

#include <escher/childless_view.h>
#include <poincare.h>

class FloatView : public ChildlessView {
public:
  FloatView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setFloat(float f);
  char * buffer();
private:
  Float m_float;
  char m_buffer[14];
  KDColor m_backgroundColor;
};

#endif
