#ifndef ESCHER_SWITCH_MENU_LIST_CELL_H
#define ESCHER_SWITCH_MENU_LIST_CELL_H

#include <escher/menu_list_cell.h>
#include <escher/switch_view.h>

class SwitchMenuListCell : public MenuListCell {
public:
  SwitchMenuListCell(char * label);
  View * accessoryView() const override;
private:
  SwitchView m_accessoryView;
};

#endif
