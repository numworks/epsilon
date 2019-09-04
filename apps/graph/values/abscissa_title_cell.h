#ifndef GRAPH_ABSCISSA_TITLE_CELL_H
#define GRAPH_ABSCISSA_TITLE_CELL_H

#include <escher/even_odd_message_text_cell.h>

namespace Graph {

// TODO LEA copied from Shared::StoreTitleCell -> refactor?

class AbscissaTitleCell : public EvenOddMessageTextCell {
public:
  AbscissaTitleCell() :
    EvenOddMessageTextCell(),
    m_separatorLeft(false)
  {}
  void setSeparatorLeft(bool separator);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
private:
  bool m_separatorLeft;
};

}

#endif
