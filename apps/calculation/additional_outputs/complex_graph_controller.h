#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CONTROLLER_H

#include "complex_graph_view.h"
#include "complex_model.h"

namespace Calculation {

class ComplexGraphController : public ViewController {
public:
  ComplexGraphController();
  const char * title() override { return "Emilie"; }
  View * view() override {
    return &m_graphView;
  }
private:
  ComplexModel m_complex;
  ComplexGraphView m_graphView;
};

}

#endif

