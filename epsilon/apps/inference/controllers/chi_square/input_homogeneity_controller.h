#pragma once

#include "inference/controllers/tables/categorical_controller.h"
#include "tables/input_homogeneity_table.h"

namespace Inference {

class InputHomogeneityController : public InputCategoricalController {
 public:
  InputHomogeneityController(Escher::StackViewController* parent,
                             ControllerContainer* controllerContainer,
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

  InputHomogeneityTable m_inputHomogeneityTable;
};

}  // namespace Inference
