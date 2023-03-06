#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, Escher::ViewController* resultsController,
    HomogeneityTest* statistic,
    InputEventHandlerDelegate* inputEventHandlerDelegate)
    : InputCategoricalController(parent, resultsController, statistic,
                                 inputEventHandlerDelegate),
      m_inputHomogeneityTable(&m_selectableListView, this, this, statistic) {}

void InputHomogeneityController::tableViewDidChangeSelection(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    bool withinTemporarySelection) {
  m_inputHomogeneityTable.unselectTopLeftCell(t, previousSelectedCol,
                                              previousSelectedRow);
  InputCategoricalController::tableViewDidChangeSelection(
      t, previousSelectedCol, previousSelectedRow, withinTemporarySelection);
}

}  // namespace Inference
