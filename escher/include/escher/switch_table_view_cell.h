#ifndef ESCHER_SWITCH_TABLE_VIEW_CELL_H
#define ESCHER_SWITCH_TABLE_VIEW_CELL_H

#include <escher/table_view_cell.h>
#include <escher/switch_view.h>

class SwitchTableViewCell : public TableViewCell {
public:
  SwitchTableViewCell(char * label);
  View * contentView() const override;
private:
  SwitchView m_contentView;
};

#endif
