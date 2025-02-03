#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_HOMOGENEITY_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_HOMOGENEITY_CONTROLLER_H

#include <escher/tab_view_controller.h>

#include "inference/statistic/categorical_controller.h"
#include "results_homogeneity_table_cell.h"

namespace Inference {

class ResultsHomogeneityController : public CategoricalController {
 public:
  ResultsHomogeneityController(Escher::ViewController* nextController,
                               HomogeneityTest* statistic);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  void viewWillAppear() override;
  void stackOpenPage(ViewController* nextPage) override {
    tabController()->stackOpenPage(nextPage);
  }

  void setMode(ResultsHomogeneityTableCell::Mode mode) {
    m_resultsHomogeneityTable.setMode(mode);
  }

  Escher::TabViewController* tabController() {
    return static_cast<Escher::TabViewController*>(
        parentResponder()->parentResponder());
  }

 private:
  CategoricalTableCell* categoricalTableCell() override {
    return &m_resultsHomogeneityTable;
  }
  void createDynamicCells() override;
  ResultsHomogeneityTableCell m_resultsHomogeneityTable;
};

class ResultsHomogeneityTabController : public Escher::TabViewController,
                                        public Escher::TabViewDataSource {
 public:
  ResultsHomogeneityTabController(Escher::StackViewController* parent,
                                  Escher::ViewController* nextController,
                                  HomogeneityTest* statistic);

  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  KDColor tabBackgroundColor() const override {
    return Escher::Palette::GrayDark;
  }

 private:
  class SingleModeController : public ViewController {
   public:
    SingleModeController(Escher::Responder* responder,
                         ResultsHomogeneityController* tableController)
        : Escher::ViewController(responder),
          m_tableController(tableController) {}
    Escher::View* view() override { return m_tableController->view(); }
    void initView() override { m_tableController->initView(); }
    void viewWillAppear() override { m_tableController->viewWillAppear(); }

   protected:
    void switchToTableWithMode(ResultsHomogeneityTableCell::Mode mode);
    ResultsHomogeneityController* m_tableController;
    void handleResponderChainEvent(
        Responder::ResponderChainEvent event) override {
      if (event.type == ResponderChainEventType::HasBecomeFirst) {
        m_tableController->didBecomeFirstResponder();
      } else {
        Escher::ViewController::handleResponderChainEvent(event);
      }
    }
  };

  class ExpectedValuesController : public SingleModeController {
   public:
    using SingleModeController::SingleModeController;
    const char* title() const override {
      return I18n::translate(
          I18n::Message::HomogeneityResultsExpectedValuesTitle);
    }
    void viewWillAppear() override {
      switchToTableWithMode(ResultsHomogeneityTableCell::Mode::ExpectedValue);
      SingleModeController::viewWillAppear();
    }
  };

  class ContributionsController : public SingleModeController {
   public:
    using SingleModeController::SingleModeController;
    const char* title() const override {
      return I18n::translate(I18n::Message::Contributions);
    }
    void viewWillAppear() override {
      switchToTableWithMode(ResultsHomogeneityTableCell::Mode::Contribution);
      SingleModeController::viewWillAppear();
    }
  };

  ResultsHomogeneityController m_tableController;
  ExpectedValuesController m_expectedValuesController;
  ContributionsController m_contributionsController;
};

}  // namespace Inference

#endif
