#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_HOMOGENEITY_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_HOMOGENEITY_CONTROLLER_H

#include "inference/statistic/chi_square_and_slope/categorical_controller.h"
#include "inference/statistic/chi_square_and_slope/result_homogeneity_table_cell.h"
#include <escher/tab_view_controller.h>

namespace Inference {

class ResultsHomogeneityController : public Escher::TabViewController, public Escher::TabViewDataSource {
public:
  ResultsHomogeneityController(Escher::StackViewController * parent, Escher::ViewController * nextController, HomogeneityTest * statistic);

  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTwoTitles; }
  /* TabViewController::initView() will create new tabs every time it's called,
   * without ever destroying them. As such, we make sure to call it only once
   * in the constructor. */
  void initView() override {}
  KDColor tabBackgroundColor() const override { return Escher::Palette::GrayDark; }


private:
  class ResultsTableController : public CategoricalController {
  public:
    ResultsTableController(Escher::ViewController * nextController, HomogeneityTest * statistic);

    // Responder
    bool handleEvent(Ion::Events::Event event) override;

    // ViewController
    void viewWillAppear() override;
    void stackOpenPage(ViewController * nextPage) override { tabController()->stackOpenPage(nextPage); }

    // SelectableTableViewDelegate
    void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
    bool canStoreContentOfCellAtLocation(Escher::SelectableTableView * t, int col, int row) const override { return col > 0 && row > 0; }

    void setMode(ResultHomogeneityTableCell::Mode mode) { m_resultHomogeneityTable.setMode(mode); }

  private:
    Escher::TabViewController * tabController() { return static_cast<Escher::TabViewController *>(parentResponder()->parentResponder()); }
    CategoricalTableCell * categoricalTableCell() override { return &m_resultHomogeneityTable; }
    ResultHomogeneityTableCell m_resultHomogeneityTable;
  };

  class SingleModeController : public ViewController {
  public:
    SingleModeController(Escher::Responder * responder, ResultsTableController * tableController) :
      Escher::ViewController(responder),
      m_tableController(tableController)
    {}
    Escher::View * view() override { return m_tableController->view(); }
    void didBecomeFirstResponder() override { m_tableController->didBecomeFirstResponder(); }
    void viewWillAppear() override { m_tableController->viewWillAppear(); }

  protected:
    void switchToTableWithMode(ResultHomogeneityTableCell::Mode mode);
    ResultsTableController * m_tableController;
  };

  class ExpectedValuesController : public SingleModeController {
  public:
    using SingleModeController::SingleModeController;
    const char * title() override { return I18n::translate(I18n::Message::HomogeneityResultsExpectedValuesTitle); }
    void viewWillAppear() override {
      switchToTableWithMode(ResultHomogeneityTableCell::Mode::ExpectedValue);
      SingleModeController::viewWillAppear();
    }
  };

  class ContributionsController : public SingleModeController {
  public:
    using SingleModeController::SingleModeController;
    const char * title() override { return I18n::translate(I18n::Message::HomogeneityResultsContributionsTitle); }
    void viewWillAppear() override {
      switchToTableWithMode(ResultHomogeneityTableCell::Mode::Contribution);
      SingleModeController::viewWillAppear();
    }
  };

  ResultsTableController m_tableController;
  ExpectedValuesController m_expectedValuesController;
  ContributionsController m_contributionsController;
};

}

#endif
