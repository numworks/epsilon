#ifndef APPS_PROBABILITY_CONTROLLERS_CATEGORICAL_TYPE_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_CATEGORICAL_TYPE_CONTROLLER_H

#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "probability/gui/selectable_cell_list_controller.h"
#include "probability/models/statistic/chi2_statistic.h"

namespace Probability {

class InputGoodnessController;
class InputHomogeneityController;

constexpr static int k_numberOfCategoricalCells = 2;

class CategoricalTypeController : public SelectableCellListPage<Escher::MessageTableCellWithChevron,
                                                                k_numberOfCategoricalCells> {
public:
  CategoricalTypeController(Escher::StackViewController * parent,
                            Chi2Statistic * statistic,
                            InputGoodnessController * inputGoodnessController,
                            InputHomogeneityController * inputHomogeneityController);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  const char * title() override { return "χ2-test"; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  constexpr static int k_indexOfGoodnessCell = 0;
  constexpr static int k_indexOfHomogeneityCell = 1;

  Chi2Statistic * m_statistic;
  InputGoodnessController * m_inputGoodnessController;
  InputHomogeneityController * m_inputHomogeneityController;
};

}  // namespace Probability
#endif /* APPS_PROBABILITY_CONTROLLERS_CATEGORICAL_TYPE_CONTROLLER_H */
