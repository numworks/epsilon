#ifndef GRAPH_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_PARAM_CONTROLLER_H

#include <escher.h>


class ParameterController : public ViewController {
public:
  ParameterController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  SolidColorView m_solidColorView;
};

#endif
