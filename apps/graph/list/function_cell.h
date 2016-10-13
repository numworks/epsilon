#ifndef GRAPH_FUNCTION_CELL_H
#define GRAPH_FUNCTION_CELL_H

#include <escher.h>
#include "../function.h"
#include "../even_odd_cell.h"

namespace Graph {

class FunctionCell : public EvenOddCell {
public:
  FunctionCell();
  void setFunction(Function * f);
  Function * function();

  static constexpr KDColor k_desactiveTextColor = KDColor(0x646464);

protected:
  Function * m_function;
};

}

#endif
