#ifndef GRAPH_VALUE_CELL_H
#define GRAPH_VALUE_CELL_H

#include <escher.h>
#include <poincare.h>
#include "../even_odd_cell.h"

namespace Graph {
class ValueCell : public EvenOddCell {
public:
  ValueCell();
  void setFloat(float f);
  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  Float m_float;
  char m_buffer[14];
};

}

#endif
