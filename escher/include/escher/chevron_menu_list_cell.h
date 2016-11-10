#ifndef ESCHER_CHEVRON_MENU_LIST_CELL_H
#define ESCHER_CHEVRON_MENU_LIST_CELL_H

#include <escher/menu_list_cell.h>
#include <escher/chevron_view.h>

class ChevronMenuListCell : public MenuListCell {
public:
  ChevronMenuListCell(char * label = nullptr);
  View * accessoryView() const override;
  void reloadCell() override;
private:
  ChevronView m_accessoryView;
};

#endif
