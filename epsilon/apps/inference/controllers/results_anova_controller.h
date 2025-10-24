#pragma once

#include <escher/tab_view_controller.h>

#include "inference/controllers/inference_controller.h"
#include "inference/controllers/tables/categorical_controller.h"
#include "inference/controllers/tables/results_anova_table_cell.h"

namespace Inference {

class ResultsANOVAController : public CategoricalController {
 public:
  ResultsANOVAController(Escher::StackViewController* parent,
                         ControllerContainer* controllerContainer,
                         ANOVATest* inferenceModel);

  const char* title() const override { return ""; }

  // TODO: titleDisplay

 private:
  CategoricalTableCell* categoricalTableCell() override {
    return &m_resultsANOVATable;
  }
  void createDynamicCells() override;

  ResultsANOVATableCell m_resultsANOVATable;
};

}  // namespace Inference
