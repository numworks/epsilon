#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher.h>

namespace Home {

class AppCell : public View {
public:
  AppCell();

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  void setVisible(bool visible);
  void setActive(bool active);
  void setApp(::App * app);
private:
  static constexpr KDCoordinate k_iconSize = 32;
  ImageView m_iconView;
  PointerTextView m_nameView;
  bool m_visible;
  bool m_active;
};

}

#endif
