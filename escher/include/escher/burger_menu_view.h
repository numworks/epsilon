#ifndef ESCHER_BURGER_MENU_VIEW_H
#define ESCHER_BURGER_MENU_VIEW_H

#include <escher/view.h>

class BurgerMenuView : public View {
public:
  BurgerMenuView() : m_backgroundColor(KDColorWhite) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setBackgroundColor(KDColor color);
  constexpr static KDCoordinate k_burgerSize = 13;
private:
  KDColor m_backgroundColor;
};

#endif
