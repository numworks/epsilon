#ifndef ESCHER_TEXT_BUFFER_MENU_LIST_CELL_H
#define ESCHER_TEXT_BUFFER_MENU_LIST_CELL_H

#include <escher/view.h>
#include <escher/buffer_text_view.h>
#include <escher/pointer_text_view.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <escher/table_view_cell.h>

class TextBufferMenuListCell : public TableViewCell {
public:
  TextBufferMenuListCell(char * accessoryText = nullptr);
  void setHighlighted(bool highlight) override;
  void setText(const char * text);
  void setAccessoryText(const char * textBody);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  constexpr static KDCoordinate k_separatorThickness = 1;
  BufferTextView m_labelTextView;
  PointerTextView m_accessoryView;
};

#endif
