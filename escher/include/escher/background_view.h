#ifndef ESCHER_BACKGROUND_VIEW_H
#define ESCHER_BACKGROUND_VIEW_H

#include <escher/view.h>

class BackgroundView : public View {
public:
  BackgroundView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundImage(const uint8_t * data);
  void setDefaultColor(KDColor defaultColor);
  void updateDataValidity();
private:
  const uint8_t * m_data;
  bool m_isDataValid;
  KDColor m_defaultColor;
};

#endif
