#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_GOODNESS_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_GOODNESS_CONTROLLER_H

#include "inference/statistic/categorical_cell.h"
#include "inference/statistic/categorical_controller.h"
#include "input_goodness_table_cell.h"

namespace Inference {

class InputGoodnessController : public InputCategoricalController {
 public:
  InputGoodnessController(Escher::StackViewController* parent,
                          Escher::ViewController* resultsController,
                          GoodnessTest* statistic);

  void updateDegreeOfFreedomCell();

  // Responder
  void viewWillAppear() override;

  // ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::InputGoodnessControllerTitle);
  }

 private:
  constexpr static int k_indexOfDegreeOfFreedom = 1;

  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_inputGoodnessTableCell;
  }
  void createDynamicCells() override;
  int indexOfSignificanceCell() const override {
    return k_indexOfDegreeOfFreedom + 1;
  }
  int indexOfEditedParameterAtIndex(int index) const override;
  const Escher::HighlightCell* privateExplicitCellAtRow(int row) const override;

  InputCategoricalCell<Escher::MessageTextView> m_degreeOfFreedomCell;
  InputGoodnessTableCell m_inputGoodnessTableCell;
};

}  // namespace Inference

#endif
