#ifndef ESCHER_POINTER_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_POINTER_TABLE_CELL_WITH_BUFFER_H

#include <escher/pointer_table_cell.h>
#include <escher/buffer_text_view.h>

class PointerTableCellWithBuffer : public PointerTableCell {
public:
  PointerTableCellWithBuffer(char * label = nullptr);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
  const char * accessoryText();
  void setTextColor(KDColor color) override;
protected:
  BufferTextView m_accessoryView;
};

#endif
