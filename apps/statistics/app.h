#ifndef STAT_APP_H
#define STAT_APP_H

#include <apps/shared/store_app.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/alternate_view_controller.h>
#include <escher/tab_union_view_controller.h>

#include "data/store_controller.h"
#include "graph/box_controller.h"
#include "graph/frequency_controller.h"
#include "graph/graph_type_controller.h"
#include "graph/graph_view_model.h"
#include "graph/histogram_controller.h"
#include "graph/normal_probability_controller.h"
#include "stats/calculation_controller.h"
#include "store.h"

namespace Statistics {

class App : public Shared::StoreApp, Escher::AlternateViewDelegate {
 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image *icon() const override;
  };

  class Snapshot : public Shared::StoreApp::Snapshot {
   public:
    Snapshot();

    App *unpack(Escher::Container *container) override;
    void reset() override;
    const Descriptor *descriptor() const override;
    uint32_t *storeVersion() { return &m_storeVersion; }
    GraphViewModel *graphViewModel() { return &m_graphViewModel; }
    UserPreferences *userPreferences() { return &m_userPreferences; }
    int8_t *selectedSeries() { return &m_selectedSeries; }
    int16_t *selectedIndex() { return &m_selectedIndex; }

   private:
    uint32_t m_storeVersion;
    UserPreferences m_userPreferences;
    GraphViewModel m_graphViewModel;
    int8_t m_selectedSeries;
    int16_t m_selectedIndex;
  };

  static App *app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }

  TELEMETRY_ID("Statistics");

  Shared::StoreController *storeController() override {
    return &m_tabs.tab<StoreTab>()->m_storeController;
  }
  Escher::InputViewController *inputViewController() {
    return &m_inputViewController;
  }
  Snapshot *snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }

 private:
  App(Snapshot *snapshot, Poincare::Context *parentContext);
  int activeViewControllerIndex() const override {
    return GraphViewModel::IndexOfGraphView(
        snapshot()->graphViewModel()->selectedGraphView());
  }
  Escher::ViewController::TitlesDisplay alternateViewTitlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::NeverDisplayOwnTitle;
  }
  void activeViewDidBecomeFirstResponder(
      Escher::ViewController *activeViewController) override;
  void didBecomeActive(Escher::Window *window) override;

  struct StoreTab : public Escher::Tab {
    StoreTab();
    static constexpr I18n::Message k_title = I18n::Message::DataTab;
    Escher::ViewController *top() override {
      return &m_storeStackViewController;
    }
    StoreController m_storeController;
    Escher::ButtonRowController m_storeHeader;  // Needed for upper margin only
    Escher::StackViewController m_storeStackViewController;
  };

  struct GraphTab : public Escher::Tab {
    GraphTab();
    static constexpr I18n::Message k_title = I18n::Message::StatisticsGraphTab;
    Escher::ViewController *top() override {
      return &m_graphMenuAlternateEmptyViewController;
    }
    NormalProbabilityController m_normalProbabilityController;
    /* NormalProbabilityController is the only DataView overriding series
     * validity It may be empty when other graph views are not */
    Escher::AlternateEmptyViewController
        m_normalProbabilityAlternateEmptyViewController;
    Escher::ButtonRowController m_normalProbabilityHeader;
    FrequencyController m_frequencyController;
    Escher::ButtonRowController m_frequencyHeader;
    BoxController m_boxController;
    Escher::ButtonRowController m_boxHeader;
    HistogramController m_histogramController;
    Escher::ButtonRowController m_histogramHeader;
    GraphTypeController m_graphTypeController;
    Escher::AlternateViewController m_graphController;
    Escher::StackViewController m_graphMenuStackViewController;
    Escher::AlternateEmptyViewController
        m_graphMenuAlternateEmptyViewController;
  };

  struct CalculationTab : public Escher::Tab {
    CalculationTab();
    static constexpr I18n::Message k_title = I18n::Message::StatTab;
    Escher::ViewController *top() override { return &m_calculationHeader; }
    CalculationController m_calculationController;
    Escher::AlternateEmptyViewController
        m_calculationAlternateEmptyViewController;
    Escher::ButtonRowController
        m_calculationHeader;  // Needed for upper margin only
  };

  Store m_store;
  Poincare::Context *m_context;
  Escher::InputViewController m_inputViewController;
  Escher::TabUnion<StoreTab, GraphTab, CalculationTab> m_tabs;
  Escher::TabUnionViewController m_tabViewController;
};

}  // namespace Statistics

#endif
