#pragma once

#include "inference/controllers/tables/categorical_controller.h"
#include "inference/models/anova_test.h"
#include "tables/input_statistics_table.h"

namespace Inference::ANOVA {

class InputStatisticsController : public InputCategoricalController {
 public:
  InputStatisticsController(Escher::StackViewController* parent,
                            ControllerContainer* controllerContainer,
                            ANOVATest* inferenceModel);

  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::SameAsPreviousPage;
  };

  void initView() override;

 private:
  int indexOfSignificanceCell() const override {
    return indexOfTableCell() + 1;
  }
  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_inputANOVATable;
  }
  void createDynamicCells() override { m_inputANOVATable.createCells(); }

  InputStatisticsTable m_inputANOVATable;
};

}  // namespace Inference::ANOVA
