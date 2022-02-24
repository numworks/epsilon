#ifndef PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H

#include "homogeneity_table_view_controller.h"
#include "probability/abstract/input_categorical_controller.h"
#include "probability/models/statistic/homogeneity_statistic.h"
#include "results_homogeneity_controller.h"

namespace Probability {

class InputHomogeneityController : public InputCategoricalController {
public:
  InputHomogeneityController(StackViewController * parent,
                             ResultsHomogeneityController * homogeneityResultsController,
                             InputEventHandlerDelegate * inputEventHandlerDelegate,
                             HomogeneityStatistic * statistic);

  const char * title() override { return I18n::translate(I18n::Message::InputHomogeneityControllerTitle); }

  void didBecomeFirstResponder() override;

  TableViewController * tableViewController() override { return &m_tableController; }

private:
  InputCategoricalView * contentView() override { return &m_contentView; }
  HomogeneityTableViewController m_tableController;
  InputCategoricalView m_contentView;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H */
