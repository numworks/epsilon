#ifndef SHARED_INTERVAL_PARAM_CONTROLLER_H
#define SHARED_INTERVAL_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval.h"
#include "float_parameter_controller.h"
#include <assert.h>

namespace Shared {

class IntervalParameterController : public Shared::FloatParameterController<double> {
public:
  IntervalParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  Interval * interval() { assert(m_interval); return m_interval; }
  void setInterval(Interval * interval);
  const char * title() override;
  void setTitle(I18n::Message title) { m_title = title; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int numberOfRows() const override;
  void setStartEndMessages(I18n::Message startMessage, I18n::Message endMessage);
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
  MessageTableCellWithEditableText m_intervalCells[k_totalNumberOfCell];
  I18n::Message m_title;
  I18n::Message m_startMessage;
  I18n::Message m_endMessage;
};

}

#endif
