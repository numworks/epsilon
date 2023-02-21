#ifndef SHARED_INTERVAL_PARAM_CONTROLLER_H
#define SHARED_INTERVAL_PARAM_CONTROLLER_H

#include <assert.h>
#include <escher/message_table_cell_with_editable_text.h>

#include "float_parameter_controller.h"
#include "interval.h"
#include "pop_up_controller.h"

namespace Shared {

class IntervalParameterController
    : public Shared::FloatParameterController<double> {
 public:
  IntervalParameterController(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate);
  Interval* interval() {
    assert(m_interval);
    return m_interval;
  }
  void setInterval(Interval* interval);
  const char* title() override;
  void setTitle(I18n::Message title) { m_title = title; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  int numberOfRows() const override;
  void setStartEndMessages(I18n::Message startMessage,
                           I18n::Message endMessage);

 protected:
  constexpr static int k_totalNumberOfCell = 3;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Interval* m_interval;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  static Interval::IntervalParameters* SharedTempIntervalParameters();
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  double parameterAtIndex(int index) override;
  void buttonAction() override;
  Escher::MessageTableCellWithEditableText m_intervalCells[k_totalNumberOfCell];
  I18n::Message m_title;
  I18n::Message m_startMessage;
  I18n::Message m_endMessage;
  Shared::MessagePopUpController m_confirmPopUpController;
};

}  // namespace Shared

#endif
