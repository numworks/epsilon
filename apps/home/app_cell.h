#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher.h>

namespace Home {

class AppCell : public TableViewCell {
public:
  AppCell();

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  void setVisible(bool visible);
  void reloadCell() override;
  void setApp(::App * app);
private:
  static constexpr KDCoordinate k_iconWidth = 55;
  static constexpr KDCoordinate k_iconHeight = 56;
  ImageView m_iconView;
  PointerTextView m_nameView;
  bool m_visible;
};

}

#endif
