#ifndef PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H

#include "inference/statistic/chi_square_test/categorical_controller.h"
#include "inference/statistic/chi_square_test/result_homogeneity_table_cell.h"

namespace Inference {

class ResultsHomogeneityController : public CategoricalController {
public:
  ResultsHomogeneityController(Responder * parent, Escher::ViewController * nextController, HomogeneityTest * statistic);

  // Responder
  void didBecomeFirstResponder() override;

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override { m_resultHomogeneityTable.unselectTopLeftCell(t, previousSelectedCellX, previousSelectedCellY); }

private:
  CategoricalTableCell * categoricalTableCell() override { return &m_resultHomogeneityTable; }

  ResultHomogeneityTableCell m_resultHomogeneityTable;
};

}  // namespace Inference

#endif /* PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H */
