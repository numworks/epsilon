#ifndef ESCHER_LIST_VIEW_CELL_H
#define ESCHER_LIST_VIEW_CELL_H

#include <escher/view.h>
#include <escher/pointer_text_view.h>
#include <escher/palette.h>
#include <escher/metric.h>


class ListViewCell : public View {
public:
  ListViewCell(char * label = nullptr);
  PointerTextView * textView();
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
  PointerTextView m_pointerTextView;
};

#endif
