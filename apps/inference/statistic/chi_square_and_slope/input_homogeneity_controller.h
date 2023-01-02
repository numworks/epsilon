#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_CONTROLLER_H

#include "inference/statistic/chi_square_and_slope/categorical_controller.h"
#include "inference/statistic/chi_square_and_slope/input_homogeneity_table_cell.h"

namespace Inference {

class InputHomogeneityController : public InputCategoricalController {
public:
  InputHomogeneityController(Escher::StackViewController * parent, Escher::ViewController * homogeneityResultsController, HomogeneityTest * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);

  // ViewController
  const char * title() override { return I18n::translate(I18n::Message::InputHomogeneityControllerTitle); }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

private:
  int indexOfSignificanceCell() const override { return k_indexOfTableCell + 1; }
  EditableCategoricalTableCell * categoricalTableCell() override { return &m_inputHomogeneityTable; }

  InputHomogeneityTableCell m_inputHomogeneityTable;
};

}

#endif
