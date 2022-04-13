#ifndef PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H

#include "probability/abstract/categorical_controller.h"
#include "probability/gui/result_homogeneity_table_cell.h"

namespace Probability {

class ResultsHomogeneityController : public CategoricalController {
public:
  ResultsHomogeneityController(StackViewController * parent, Escher::ViewController * nextController, HomogeneityTest * statistic);

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

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H */
