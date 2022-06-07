#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_GOODNESS_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_GOODNESS_CONTROLLER_H

#include "inference/statistic/chi_square_and_slope/categorical_controller.h"
#include "inference/statistic/chi_square_and_slope/goodness_table_cell.h"

namespace Inference {

class InputGoodnessController : public InputCategoricalController {
public:
  InputGoodnessController(Escher::StackViewController * parent, Escher::ViewController * resultsController, GoodnessTest * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);

  void updateDegreeOfFreedomCell();

  // Responder
  void didBecomeFirstResponder() override;

  // ViewController
  const char * title() override { return I18n::translate(I18n::Message::InputGoodnessControllerTitle); }

  // ListViewDataSource
  virtual Escher::HighlightCell * reusableCell(int index, int type) override;

private:
  constexpr static int k_indexOfDegreeOfFreedom = 1;

  EditableCategoricalTableCell * categoricalTableCell() override { return &m_goodnessTableCell; }
  int indexOfSignificanceCell() const override { return k_indexOfDegreeOfFreedom + 1; }
  int indexOfEditedParameterAtIndex(int index) const override;

  Escher::MessageTableCellWithEditableTextWithMessage m_innerDegreeOfFreedomCell;
  Escher::CellWithMargins m_degreeOfFreedomCell;
  GoodnessTableCell m_goodnessTableCell;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_GOODNESS_CONTROLLER_H */
