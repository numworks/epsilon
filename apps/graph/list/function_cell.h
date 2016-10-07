#ifndef GRAPH_FUNCTION_CELL_H
#define GRAPH_FUNCTION_CELL_H

#include <escher.h>
#include "../function.h"
#include "../even_odd_cell.h"

class FunctionCell : public EvenOddCell {
public:
  FunctionCell();
  void setFunction(Graph::Function * f);
  Graph::Function * function();

  static constexpr KDColor k_desactiveTextColor = KDColor(0x646464);

protected:
  Graph::Function * m_function;
};

#endif
