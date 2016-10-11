#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "../function_store.h"

class GraphController : public HeaderViewController {
public:
  GraphController(Responder * parentResponder, Graph::FunctionStore * functionStore);
  const char * title() const override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  Responder * tabController() const;
  GraphView m_view;
  bool m_headerSelected;
};

#endif
