#ifndef GRAPH_NEW_FUNCTION_CELL_H
#define GRAPH_NEW_FUNCTION_CELL_H

#include <escher.h>
#include "../even_odd_cell.h"

namespace Graph {
class NewFunctionCell : public EvenOddCell {
public:
  NewFunctionCell();
  void reloadCell() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

protected:
  PointerTextView m_pointerTextView;
};

}

#endif
