#pragma once

#include "inference/controllers/categorical_controller.h"
#include "input_homogeneity_table_cell.h"

namespace Inference {

class InputHomogeneityController : public InputCategoricalController {
 public:
  InputHomogeneityController(
      Escher::StackViewController* parent,
      Escher::ViewController* homogeneityResultsController,
      HomogeneityTest* inference);

  // ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::InputHomogeneityControllerTitle);
  }

 private:
  int indexOfSignificanceCell() const override {
    return indexOfTableCell() + 1;
  }
  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_inputHomogeneityTable;
  }
  void createDynamicCells() override;

  InputHomogeneityTableCell m_inputHomogeneityTable;
};

}  // namespace Inference
