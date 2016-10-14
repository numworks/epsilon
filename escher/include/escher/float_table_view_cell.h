#ifndef ESCHER_FLOAT_TABLE_VIEW_CELL_H
#define ESCHER_FLOAT_TABLE_VIEW_CELL_H

#include <escher/table_view_cell.h>
#include <escher/float_view.h>

class FloatTableViewCell : public TableViewCell {
public:
  FloatTableViewCell(char * label);
  View * contentView() const override;
  void setHighlighted(bool highlight);
  void setFloat(float f);
  char * stringFromFloat();
private:
  FloatView m_contentView;
};

#endif
