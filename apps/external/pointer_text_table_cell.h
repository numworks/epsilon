#ifndef ESCHER_POINTER_TEXT_TABLE_CELL_H
#define ESCHER_POINTER_TEXT_TABLE_CELL_H

#include <escher/pointer_text_view.h>
#include <escher/table_cell.h>

class PointerTextTableCell : public TableCell {
public:
  PointerTextTableCell(const char * text = "", const KDFont * font = KDFont::SmallFont, Layout layout = Layout::Horizontal);
  View * labelView() const override;
  const char * text() const override;
  virtual void setHighlighted(bool highlight) override;
  void setText(const char * text);
  virtual void setTextColor(KDColor color);
  void setTextFont(const KDFont * font);
private:
  PointerTextView m_pointerTextView;
};

#endif
