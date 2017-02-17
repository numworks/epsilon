#ifndef ESCHER_MENU_LIST_CELL_H
#define ESCHER_MENU_LIST_CELL_H

#include <escher/view.h>
#include <escher/pointer_text_view.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <escher/table_view_cell.h>

class MenuListCell : public TableViewCell {
public:
  MenuListCell(char * label = nullptr, KDText::FontSize size = KDText::FontSize::Small);
  virtual View * accessoryView() const;
  void setText(const char * text);
  virtual void setTextColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlighted(bool highlight) override;
protected:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  constexpr static KDCoordinate k_separatorThickness = 1;
private:
  PointerTextView m_pointerTextView;
};

#endif
