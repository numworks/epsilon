#ifndef ESCHER_TABLE_VIEW_CELL_H
#define ESCHER_TABLE_VIEW_CELL_H

#include <escher/view.h>
#include <escher/text_view.h>
#include <escher/palette.h>
#include <escher/metric.h>


class TableViewCell : public View {
public:
  TableViewCell(char * label);
  TextView * textView();
  virtual View * contentView() const;
  bool isHighlighted() const;
  void setHighlighted(bool highlight);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  bool m_highlighted;
  TextView m_textView;
};

#endif
