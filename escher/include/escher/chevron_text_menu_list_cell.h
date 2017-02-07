#ifndef ESCHER_CHEVRON_TEXT_MENU_LIST_CELL_H
#define ESCHER_CHEVRON_TEXT_MENU_LIST_CELL_H

#include <escher/chevron_menu_list_cell.h>

class ChevronTextMenuListCell : public ChevronMenuListCell {
public:
  ChevronTextMenuListCell(KDText::FontSize size = KDText::FontSize::Small);
  void reloadCell() override;
  void setSubtitle(const char * text);
private:
  static constexpr KDCoordinate k_margin = 8;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_subtitleView;
};

#endif
