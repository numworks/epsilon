#ifndef GRAPH_GRAPH_GOTO_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_GOTO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "axis_interval.h"
#include "graph_view.h"
#include "../float_parameter_controller.h"

namespace Graph {
class GoToParameterController : public FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, GraphView * graphView);
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void setFunction(Function * function);
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  TextMenuListCell m_abscisseCell;
  GraphView * m_graphView;
  Function * m_function;
};

}

#endif
