#ifndef ESCHER_TEXT_LIST_VIEW_CELL_H
#define ESCHER_TEXT_LIST_VIEW_CELL_H

#include <escher/list_view_cell.h>
#include <escher/buffer_text_view.h>

class TextListViewCell : public ListViewCell {
public:
  TextListViewCell(char * label);
  void reloadCell() override;
  View * contentView() const override;
  void setHighlighted(bool highlight);
  void setText(const char * textBody);
  const char * textContent();
private:
  BufferTextView m_contentView;
};

#endif
