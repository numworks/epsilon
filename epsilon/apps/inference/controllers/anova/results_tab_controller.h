#pragma once

#include <escher/responder.h>
#include <escher/tab_view_controller.h>

#include "inference/controllers/inference_controller.h"
#include "inference/controllers/tables/categorical_controller.h"
#include "tables/results_between_within_table.h"
#include "tables/results_statistics_table.h"

namespace Inference::ANOVA {

/* This controller displays the "between" and "within" statistics results
 * (degrees of freedom, square sum, mean squares) of the ANOVA test. */
class BetweenWithinController : public CategoricalController {
 public:
  BetweenWithinController(Escher::Responder* parentResponder,
                          ControllerContainer* controllerContainer,
                          ANOVATest* inference, bool isStandalone);

  void stackOpenPage(ViewController* nextPage) override {
    if (m_isStandalone) {
      CategoricalController::stackOpenPage(nextPage);
    } else {
      tabController()->stackOpenPage(nextPage);
    }
  }

  Escher::TabViewController* tabController() {
    assert(!m_isStandalone);
    return static_cast<Escher::TabViewController*>(parentResponder());
  }

  // Escher::ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::CalculatedValues);
  }

  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::SameAsPreviousPage;
  };

 private:
  virtual CategoricalTableCell* categoricalTableCell() override {
    return &m_resultsBetweenWithinTable;
  }
  void createDynamicCells() override;

  ResultsBetweenWithinTableCell m_resultsBetweenWithinTable;

  // False if integrated in a tab controller, true otherwise
  bool m_isStandalone;
};

class StatisticsController : public CategoricalController {
 public:
  StatisticsController(Escher::Responder* parentResponder,
                       ControllerContainer* controllerContainer,
                       ANOVATest* inference);

  void stackOpenPage(ViewController* nextPage) override {
    tabController()->stackOpenPage(nextPage);
  }

  Escher::TabViewController* tabController() {
    return static_cast<Escher::TabViewController*>(parentResponder());
  }

  // Escher::ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::StatsApp);
  }

 private:
  virtual CategoricalTableCell* categoricalTableCell() override {
    return &m_resultsStatisticsTable;
  }
  void createDynamicCells() override;

  ResultsStatisticsTable m_resultsStatisticsTable;
};

class ResultsTabController : public InferenceController,
                             public Escher::TabViewController,
                             public Escher::TabViewDataSource {
 public:
  ResultsTabController(Escher::StackViewController* parent,
                       ControllerContainer* controllerContainer,
                       ANOVATest* inferenceModel);

  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::SameAsPreviousPage;
  };

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  KDColor tabBackgroundColor() const override {
    return Escher::Palette::GrayDarkMiddle;
  }

 private:
  BetweenWithinController m_firstTabController;
  StatisticsController m_secondTabController;
};

}  // namespace Inference::ANOVA
