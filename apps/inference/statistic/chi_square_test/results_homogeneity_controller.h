#ifndef PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H

#include "inference/statistic/chi_square_test/categorical_controller.h"
#include "inference/statistic/chi_square_test/result_homogeneity_table_cell.h"
#include <escher/tab_view_controller.h>

namespace Inference {

class ResultsHomogeneityController : public Escher::TabViewController, public Escher::TabViewDataSource {
public:
  ResultsHomogeneityController(Escher::StackViewController * parent, Escher::ViewController * nextController, HomogeneityTest * statistic);

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTwoTitles; }

private:
  class ResultsTableController : public CategoricalController {
  public:
    ResultsTableController(Responder * parent, Escher::ViewController * nextController, HomogeneityTest * statistic);
    // Responder
    void didBecomeFirstResponder() override;
    bool handleEvent(Ion::Events::Event event) override { return typedParent()->popFromStackViewControllerOnLeftEvent(event); }
    // ViewController
    void stackOpenPage(ViewController * nextPage) override { typedParent()->stackOpenPage(nextPage); }

    // SelectableTableViewDelegate
    void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override { m_resultHomogeneityTable.unselectTopLeftCell(t, previousSelectedCellX, previousSelectedCellY); }

    void setMode(ResultHomogeneityTableCell::Mode mode) { m_resultHomogeneityTable.setMode(mode); }

  private:
    ResultsHomogeneityController * typedParent() const { return static_cast<ResultsHomogeneityController *>(parentResponder()); }

    CategoricalTableCell * categoricalTableCell() override { return &m_resultHomogeneityTable; }
    ResultHomogeneityTableCell m_resultHomogeneityTable;
  };

  class SingleModeController : public ViewController {
  public:
    using ViewController::ViewController;
    View * view() override { return typedParent()->m_tableController.view(); }

  protected:
    ResultsHomogeneityController * typedParent() const { return static_cast<ResultsHomogeneityController *>(parentResponder()); }
    void switchToTableWithMode(ResultHomogeneityTableCell::Mode mode);
  };

  class ExpectedValuesController : public SingleModeController {
  public:
    using SingleModeController::SingleModeController;
    const char * title() override { return "E.V."; } // TODO
    void didBecomeFirstResponder() override { switchToTableWithMode(ResultHomogeneityTableCell::Mode::ExpectedValue); }
  };

  class ContributionsController : public SingleModeController {
  public:
    using SingleModeController::SingleModeController;
    const char * title() override { return "Cont."; } // TODO
    void didBecomeFirstResponder() override { switchToTableWithMode(ResultHomogeneityTableCell::Mode::Contribution); }
  };

  ResultsTableController m_tableController;
  ExpectedValuesController m_expectedValuesController;
  ContributionsController m_contributionsController;
};

}  // namespace Inference

#endif /* PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H */
