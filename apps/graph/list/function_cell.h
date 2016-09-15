#ifndef GRAPH_FUNCTION_CELL_H
#define GRAPH_FUNCTION_CELL_H

#include <escher.h>
#include "../function.h"

class FunctionCell : public ChildlessView, public Responder {
public:
  FunctionCell();
  void setFunction(Graph::Function * f);
  void setEven(bool even);

  void drawRect(KDContext * ctx, KDRect rect) const override;
  void didBecomeFirstResponder() override;
  void didResignFirstResponder() override;
private:
  Graph::Function * m_function;
  bool m_focused;
  bool m_even;
};

#endif
