#pragma once

#include "inference/controllers/tables/categorical_cell.h"
#include "inference/controllers/tables/categorical_controller.h"
#include "tables/input_goodness_table.h"

namespace Inference {

class InputGoodnessController : public InputCategoricalController {
 public:
  InputGoodnessController(Escher::StackViewController* parent,
                          ControllerContainer* controllerContainer,
                          GoodnessTest* inference);

  void updateDegreeOfFreedomCell();

  // Responder
  void viewWillAppear() override;

  // ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::InputGoodnessControllerTitle);
  }

 private:
  constexpr static int k_indexOfDegreesOfFreedom = 1;

  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_InputGoodnessTable;
  }
  void createDynamicCells() override;
  int indexOfSignificanceCell() const override {
    return k_indexOfDegreesOfFreedom + 1;
  }
  int indexOfEditedParameterAtIndex(int index) const override;
  const Escher::HighlightCell* privateExplicitCellAtRow(int row) const override;

  InputCategoricalCell<Escher::MessageTextView> m_degreeOfFreedomCell;
  InputGoodnessTable m_InputGoodnessTable;
};

}  // namespace Inference
