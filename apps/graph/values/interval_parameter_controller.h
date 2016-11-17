#ifndef GRAPH_INTERVAL_PARAM_CONTROLLER_H
#define GRAPH_INTERVAL_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval.h"
#include "../float_parameter_controller.h"

namespace Graph {

class IntervalParameterController : public FloatParameterController {
public:
  IntervalParameterController(Responder * parentResponder, Interval * interval);
  Interval * interval();
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_totalNumberOfCell = 3;
  Interval * m_interval;
  TextMenuListCell m_intervalStartCell;
  TextMenuListCell m_intervalEndCell;
  TextMenuListCell m_intervalStepCell;
};

}

#endif
