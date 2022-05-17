#ifndef PROBABILITY_CONTROLLERS_INPUT_SLOPE_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_INPUT_SLOPE_CONTROLLER_H

#include "inference/statistic/chi_square_and_slope/categorical_controller.h"
#include "inference/statistic/chi_square_and_slope/slope_table_cell.h"

using namespace Escher;

namespace Inference {

class InputSlopeController : public InputCategoricalController {
public:
  InputSlopeController(StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Table * tableModel, InputEventHandlerDelegate * inputEventHandlerDelegate);

  // ViewController
  const char * title() override;

private:
  EditableCategoricalTableCell * categoricalTableCell() override { return &m_slopeTableCell; }
  int indexOfSignificanceCell() const override { return k_indexOfTableCell + 1; }

  // TODO: check longuest message?
  constexpr static int k_titleBufferSize = sizeof("t-interval for a slope");
  char m_titleBuffer[k_titleBufferSize];
  Escher::MessageTableCellWithEditableTextWithMessage m_innerDegreeOfFreedomCell;
  SlopeTableCell m_slopeTableCell;
};

}  // namespace Inference

#endif /* PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H */
