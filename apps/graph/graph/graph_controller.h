#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "../function_store.h"

class GraphController : public ViewController {
public:
  GraphController(Responder * parentResponder, Graph::FunctionStore * functionStore);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  GraphView m_view;
};

#endif
