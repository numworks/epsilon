#ifndef ESCHER_TEXT_TABLE_VIEW_CELL_H
#define ESCHER_TEXT_TABLE_VIEW_CELL_H

#include <escher/table_view_cell.h>
#include <escher/buffer_text_view.h>

class TextTableViewCell : public TableViewCell {
public:
  TextTableViewCell(char * label);
  View * contentView() const override;
  void setHighlighted(bool highlight);
  void setText(const char * textBody);
  const char * textContent();
private:
  BufferTextView m_contentView;
};

#endif
