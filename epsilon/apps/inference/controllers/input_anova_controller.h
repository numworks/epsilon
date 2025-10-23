#pragma once

#include "inference/controllers/tables/categorical_controller.h"
#include "inference/controllers/tables/input_anova_table_cell.h"
#include "inference/models/anova_test.h"

namespace Inference {

class InputANOVAController : public InputCategoricalController {
 public:
  InputANOVAController(Escher::StackViewController* parent,
                       ControllerContainer* controllerContainer,
                       ANOVATest* inferenceModel);

  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::SameAsPreviousPage;
  };

 private:
  int indexOfSignificanceCell() const override {
    return indexOfTableCell() + 1;
  }
  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_inputANOVATable;
  }
  void createDynamicCells() override;

  InputANOVATableCell m_inputANOVATable;
};

}  // namespace Inference
