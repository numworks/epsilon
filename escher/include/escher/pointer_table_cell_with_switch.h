#ifndef ESCHER_POINTER_TABLE_CELL_WITH_SWITCH_H
#define ESCHER_POINTER_TABLE_CELL_WITH_SWITCH_H

#include <escher/pointer_table_cell.h>
#include <escher/switch_view.h>

class PointerTableCellWithSwitch : public PointerTableCell {
public:
  PointerTableCellWithSwitch(char * label = nullptr);
  View * accessoryView() const override;
private:
  SwitchView m_accessoryView;
};

#endif
