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
  void viewWillAppear() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int numberOfRows() override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float previousParameterAtIndex(int index) override;
  float parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_totalNumberOfCell = 3;
  Interval * m_interval;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText m_intervalCells[k_totalNumberOfCell];
  float m_previousParameters[k_totalNumberOfCell];
};

}

#endif
