#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "../function_store.h"
#include "../evaluate_context.h"

namespace Graph {
class GraphController : public HeaderViewController {
public:
  GraphController(Responder * parentResponder, FunctionStore * functionStore, EvaluateContext * evaluateContext);
  const char * title() const override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;
private:
  Responder * tabController() const;
  GraphView m_view;
  bool m_headerSelected;
  Button m_windowButton;
  Button m_displayButton;
};

}

#endif
