#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_GOODNESS_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_GOODNESS_CONTROLLER_H

#include <escher/tab_view_controller.h>

#include "inference/statistic/categorical_controller.h"
#include "inference/statistic/results_controller.h"
#include "results_goodness_table_cell.h"

namespace Inference {

class ResultsGoodnessTabController : public Escher::TabViewController,
                                     public Escher::TabViewDataSource {
 public:
  ResultsGoodnessTabController(Escher::Responder* parent,
                               TestGraphController* testGraphController,
                               IntervalGraphController* intervalGraphController,
                               GoodnessTest* statistic);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  KDColor tabBackgroundColor() const override {
    return Escher::Palette::GrayDark;
  }

 private:
  class MainResultsController : public ResultsController {
   public:
    using ResultsController::ResultsController;

    void stackOpenPage(ViewController* nextPage) override {
      tabController()->stackOpenPage(nextPage);
    }

    const char* title() const override;

    Escher::TabViewController* tabController() {
      return static_cast<Escher::TabViewController*>(parentResponder());
    }
  };
  class ContributionsController : public CategoricalController {
   public:
    ContributionsController(Escher::Responder* parentResponder,
                            Escher::ViewController* nextController,
                            GoodnessTest* statistic);
    // ViewController
    void viewWillAppear() override;
    void stackOpenPage(ViewController* nextPage) override {
      tabController()->stackOpenPage(nextPage);
    }

    Escher::TabViewController* tabController() {
      return static_cast<Escher::TabViewController*>(parentResponder());
    }

    // Escher::ViewController
    const char* title() const override {
      return I18n::translate(I18n::Message::Contributions);
    }

   private:
    virtual CategoricalTableCell* categoricalTableCell() override {
      return &m_table;
    }
    void createDynamicCells() override;

    ResultGoodnessContributionsTable m_table;
  };
  // 1st tab
  MainResultsController m_resultsController;
  // 2nd tab
  ContributionsController m_contributionsController;
};

}  // namespace Inference

#endif
