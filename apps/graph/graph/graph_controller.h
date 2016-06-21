#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"

class GraphController : public ViewController {
public:
  GraphController();
  View * view() override;
  const char * title() const override;
  void setFocused(bool focused) override;
  bool handleEvent(ion_event_t event) override;
private:
  GraphView m_view;
};

#endif
