#ifndef SHARED_INTERVAL_PARAM_CONTROLLER_H
#define SHARED_INTERVAL_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval.h"
#include "float_parameter_controller.h"

namespace Shared {

class IntervalParameterController : public Shared::FloatParameterController {
public:
  IntervalParameterController(Responder * parentResponder, Interval * interval);
  Interval * interval();
  const char * title() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int numberOfRows() override;
protected:
  constexpr static int k_totalNumberOfCell = 3;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Interval * m_interval;
  bool handleEvent(Ion::Events::Event event) override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  double parameterAtIndex(int index) override;
  void buttonAction() override;
  View * loadView() override;
  void unloadView(View * view) override;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText * m_intervalCells[k_totalNumberOfCell];
};

}

#endif
