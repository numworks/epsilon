#ifndef PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H

#include <escher/stack_view_controller.h>

#include "goodness_table_view_controller.h"
#include "probability/abstract/input_categorical_controller.h"
#include "probability/abstract/input_goodness_view.h"
#include "probability/models/statistic/goodness_statistic.h"
#include "results_controller.h"

using namespace Escher;

namespace Probability {

class InputGoodnessController : public InputCategoricalController {
public:
  InputGoodnessController(StackViewController * parent,
                          ResultsController * resultsController,
                          GoodnessStatistic * statistic,
                          InputEventHandlerDelegate * inputEventHandlerDelegate);
  const char * title() override {
    return I18n::translate(I18n::Message::InputGoodnessControllerTitle);
  }
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                const char * text,
                                Ion::Events::Event event) override;

  void didBecomeFirstResponder() override;

  TableViewController * tableViewController() override { return &m_tableController; }

private:
  InputCategoricalView * contentView() override { return &m_contentView; }
  GoodnessTableViewController m_tableController;
  InputGoodnessView m_contentView;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H */
