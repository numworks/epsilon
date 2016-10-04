#ifndef GRAPH_FUNCTION_CELL_H
#define GRAPH_FUNCTION_CELL_H

#include <escher.h>
#include "../function.h"

class FunctionCell : public ChildlessView {
public:
  FunctionCell();
  void setFunction(Graph::Function * f);
  void setEven(bool even);
  void setHighlighted(bool highlight);
  Graph::Function * function();
  void reloadCell();

  static constexpr KDColor k_evenLineBackgroundColor = KDColor(0xF9F9F9);
  static constexpr KDColor k_oddLineBackgroundColor = KDColor(0xEEEEF2);
  static constexpr KDColor k_selectedLineBackgroundColor = KDColor(0x8582DB);
  static constexpr KDColor k_desactiveTextColor = KDColor(0x646464);

protected:
  bool m_highlighted;
  bool m_even;
  Graph::Function * m_function;
};

#endif
