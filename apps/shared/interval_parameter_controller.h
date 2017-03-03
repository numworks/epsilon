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
  const char * title() const override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_totalNumberOfCell = 3;
  Interval * m_interval;
  char m_draftTextBuffer[PointerTableCellWithEditableText::k_bufferLength];
  PointerTableCellWithEditableText m_intervalStartCell;
  PointerTableCellWithEditableText m_intervalEndCell;
  PointerTableCellWithEditableText m_intervalStepCell;
};

}

#endif
