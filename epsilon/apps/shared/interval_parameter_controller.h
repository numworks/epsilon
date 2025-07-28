#ifndef SHARED_INTERVAL_PARAM_CONTROLLER_H
#define SHARED_INTERVAL_PARAM_CONTROLLER_H

#include <assert.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "float_parameter_controller.h"
#include "interval.h"
#include "pop_up_controller.h"

namespace Shared {

class IntervalParameterController : public FloatParameterController<double> {
 public:
  IntervalParameterController(Escher::Responder* parentResponder);
  Interval* interval() {
    assert(m_interval);
    return m_interval;
  }
  void setInterval(Interval* interval);
  const char* title() const override;
  void setTitle(I18n::Message title) { m_title = title; }
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void setStartEndMessages(I18n::Message startMessage,
                           I18n::Message endMessage);

 protected:
  constexpr static int k_totalNumberOfCell = 3;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Interval* m_interval;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  static Interval::IntervalParameters* SharedTempIntervalParameters();
  int reusableParameterCellCount(int type) const override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  double parameterAtIndex(int index) override;
  void buttonAction() override;
  Escher::MenuCellWithEditableText<Escher::MessageTextView>
      m_intervalCells[k_totalNumberOfCell];
  I18n::Message m_title;
  MessagePopUpController m_confirmPopUpController;
};

}  // namespace Shared

#endif
