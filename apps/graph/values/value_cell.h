#ifndef GRAPH_VALUE_CELL_H
#define GRAPH_VALUE_CELL_H

#include <escher.h>
#include "../even_odd_cell.h"

class ValueCell : public EvenOddCell {
public:
  ValueCell();
  void setFloat(float f);
  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  float m_float;
  char m_buffer[14];
};

#endif
