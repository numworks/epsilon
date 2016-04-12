#ifndef ESCHER_SOLID_COLOR_VIEW_H
#define ESCHER_SOLID_COLOR_VIEW_H

#include <escher/view.h>

class SolidColorView : public View {
public:
  SolidColorView(KDRect frame, KDColor color);
  void drawRect(KDRect rect) override;
private:
  KDColor m_color;
};

#endif
