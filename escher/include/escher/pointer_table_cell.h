#ifndef ESCHER_POINTER_TABLE_CELL_H
#define ESCHER_POINTER_TABLE_CELL_H

#include <escher/pointer_text_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>

class PointerTableCell : public TableCell {
public:
  PointerTableCell(I18n::Message label = (I18n::Message)0, KDText::FontSize size = KDText::FontSize::Small, Layout layout = Layout::Horizontal);
  View * labelView() const override;
  virtual void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message message);
  virtual void setTextColor(KDColor color);
private:
  PointerTextView m_pointerTextView;
};

#endif
