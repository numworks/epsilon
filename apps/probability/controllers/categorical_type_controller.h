#ifndef CATEGORICAL_TYPE_CONTROLLER_H
#define CATEGORICAL_TYPE_CONTROLLER_H

#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "probability/gui/selectable_cell_list_controller.h"

namespace Probability {

class InputGoodnessController;
class InputHomogeneityController;

constexpr static int k_numberOfCategoricalCells = 2;

class CategoricalTypeController : public SelectableCellListPage<Escher::MessageTableCellWithChevron,
                                                                k_numberOfCategoricalCells> {
public:
  CategoricalTypeController(Escher::StackViewController * parent,
                            InputGoodnessController * inputGoodnessController,
                            InputHomogeneityController * inputHomogeneityController);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  constexpr static int k_indexOfGoodness = 0;
  constexpr static int k_indexOfHomogeneity = 1;

  InputGoodnessController * m_inputGoodnessController;
  InputHomogeneityController * m_inputHomogeneityController;
};

}  // namespace Probability
#endif /* CATEGORICAL_TYPE_CONTROLLER_H */
