#ifndef SETTINGS_MENU_CELL_H
#define SETTINGS_MENU_CELL_H

#include <escher.h>

namespace Settings {

class MenuCell : public ChevronMenuListCell {
public:
  MenuCell();
  void reloadCell() override;
  void setSubtitle(const char * text);
private:
  static constexpr KDCoordinate k_margin = 8;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_subtitleView;
};

}

#endif
