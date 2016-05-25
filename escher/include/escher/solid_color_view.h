#ifndef ESCHER_SOLID_COLOR_VIEW_H
#define ESCHER_SOLID_COLOR_VIEW_H

#include <escher/childless_view.h>

class SolidColorView : public ChildlessView {
public:
  SolidColorView(KDColor color);
  void drawRect(KDRect rect) override;
private:
  KDColor m_color;
};

#endif
