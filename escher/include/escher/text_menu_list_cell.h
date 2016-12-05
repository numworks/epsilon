#ifndef ESCHER_TEXT_MENU_LIST_CELL_H
#define ESCHER_TEXT_MENU_LIST_CELL_H

#include <escher/menu_list_cell.h>
#include <escher/buffer_text_view.h>

class TextMenuListCell : public MenuListCell {
public:
  TextMenuListCell(char * label = nullptr);
  void reloadCell() override;
  View * accessoryView() const override;
  void setHighlighted(bool highlight);
  void setAccessoryText(const char * textBody);
  const char * accessoryText();
  void setTextColor(KDColor color) override;
protected:
  BufferTextView m_accessoryView;
};

#endif
