#ifndef ESCHER_SWITCH_LIST_VIEW_CELL_H
#define ESCHER_SWITCH_LIST_VIEW_CELL_H

#include <escher/list_view_cell.h>
#include <escher/switch_view.h>

class SwitchListViewCell : public ListViewCell {
public:
  SwitchListViewCell(char * label);
  View * contentView() const override;
private:
  SwitchView m_contentView;
};

#endif
