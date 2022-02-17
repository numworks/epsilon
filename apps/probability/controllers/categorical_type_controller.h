#ifndef PROBABILITY_CONTROLLERS_CATEGORICAL_TYPE_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_CATEGORICAL_TYPE_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "probability/models/statistic/chi2_test.h"

namespace Probability {

class InputGoodnessController;
class InputHomogeneityController;

constexpr static int k_numberOfCategoricalCells = 2;

class CategoricalTypeController : public Escher::SelectableCellListPage<Escher::MessageTableCellWithChevron,
                                                                k_numberOfCategoricalCells> {
public:
  CategoricalTypeController(Escher::StackViewController * parent,
                            Chi2Test * statistic,
                            InputGoodnessController * inputGoodnessController,
                            InputHomogeneityController * inputHomogeneityController);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  const char * title() override {
    return I18n::translate(I18n::Message::CategoricalTypeControllerTitle);
  }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  constexpr static int k_indexOfGoodnessCell = 0;
  constexpr static int k_indexOfHomogeneityCell = 1;

private:
  Chi2Test * m_statistic;
  InputGoodnessController * m_inputGoodnessController;
  InputHomogeneityController * m_inputHomogeneityController;
};

}  // namespace Probability
#endif /* PROBABILITY_CONTROLLERS_CATEGORICAL_TYPE_CONTROLLER_H */
