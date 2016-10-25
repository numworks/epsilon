#ifndef ESCHER_TEXT_MENU_LIST_CELL_H
#define ESCHER_TEXT_MENU_LIST_CELL_H

#include <escher/menu_list_cell.h>
#include <escher/buffer_text_view.h>

class TextMenuListCell : public MenuListCell {
public:
  TextMenuListCell(char * label);
  void reloadCell() override;
  View * contentView() const override;
  void setHighlighted(bool highlight);
  void setText(const char * textBody);
  const char * textContent();
private:
  BufferTextView m_contentView;
};

#endif
