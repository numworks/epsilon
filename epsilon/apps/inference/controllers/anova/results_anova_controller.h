#pragma once

#include <escher/tab_view_controller.h>

#include "inference/controllers/inference_controller.h"
#include "inference/controllers/tables/categorical_controller.h"
#include "tables/results_between_within_table_cell.h"

namespace Inference {

class ResultsANOVAController : public CategoricalController {
 public:
  ResultsANOVAController(Escher::StackViewController* parent,
                         ControllerContainer* controllerContainer,
                         ANOVATest* inferenceModel);

  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::SameAsPreviousPage;
  };

  // TODO: titleDisplay

 private:
  CategoricalTableCell* categoricalTableCell() override {
    return &m_resultsBetweenWithinTable;
  }
  void createDynamicCells() override;

  ResultsBetweenWithinTableCell m_resultsBetweenWithinTable;
};

}  // namespace Inference
