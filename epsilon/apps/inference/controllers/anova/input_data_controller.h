#pragma once

#include "inference/controllers/tables/categorical_controller.h"
#include "inference/models/anova_test.h"
#include "tables/input_data_table_cell.h"

namespace Inference::ANOVA {

class InputDataController : public InputCategoricalController {
 public:
  InputDataController(Escher::StackViewController* parent,
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

  InputDataTableCell m_inputANOVATable;
};

}  // namespace Inference::ANOVA
