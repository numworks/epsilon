#pragma once

#include <apps/shared/store_app.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/alternate_view_controller.h>
#include <escher/tab_union_view_controller.h>

#include "categorical/data/store.h"
#include "categorical/data/store_controller.h"
#include "categorical/graph/bar_graph_controller.h"
#include "categorical/graph/graph_type_controller.h"
#include "categorical/graph/pie_graph_controller.h"
#include "data/store_controller.h"
#include "graph/box_controller.h"
#include "graph/frequency_controller.h"
#include "graph/graph_type_controller.h"
#include "graph/graph_view_model.h"
#include "graph/histogram_main_controller.h"
#include "graph/normal_probability_controller.h"
#include "statistics_type_controller.h"
#include "stats/calculation_controller.h"
#include "store.h"

namespace Statistics {

class App : public Shared::StoreApp,
            public Escher::AlternateViewDelegate,
            public Escher::AlternateEmptyViewDelegate {
  friend class DataTypeController;

 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image* icon() const override;
  };

  class Snapshot : public Shared::StoreApp::Snapshot {
   public:
    Snapshot();

    App* unpack(Escher::Container* container) override;
    void reset() override;
    void countryWasUpdated() override;
    const Descriptor* descriptor() const override;
    uint32_t* storeVersion() { return &m_storeVersion; }
    GraphViewModel* graphViewModel() { return &m_graphViewModel; }
    const GraphViewModel* graphViewModel() const { return &m_graphViewModel; }
    Categorical::GraphViewModel* categoricalGraphViewModel() {
      return &m_categoricalGraphViewModel;
    }
    const Categorical::GraphViewModel* categoricalGraphViewModel() const {
      return &m_categoricalGraphViewModel;
    }
    DataTypeViewModel* dataTypeViewModel() { return &m_dataTypeViewModel; }
    const DataTypeViewModel* dataTypeViewModel() const {
      return &m_dataTypeViewModel;
    }
    const Categorical::Store::TableData* categoricalTableData() const {
      return &m_categoricalTableData;
    }
    Categorical::Store::TableData* categoricalTableData() {
      return &m_categoricalTableData;
    }
    UserPreferences* userPreferences() { return &m_userPreferences; }
    int8_t selectedSeries() const { return m_selectedSeries; }
    int16_t selectedIndex() const { return m_selectedIndex; }

    void setSelectedSeries(int8_t selectedSeries) {
      m_selectedSeries = selectedSeries;
    }

    void setSelectedIndex(int16_t selectedIndex) {
      m_selectedIndex = selectedIndex;
    }

   private:
    Categorical::Store::TableData m_categoricalTableData;
    uint32_t m_storeVersion;
    UserPreferences m_userPreferences;
    GraphViewModel m_graphViewModel;
    Categorical::GraphViewModel m_categoricalGraphViewModel;
    DataTypeViewModel m_dataTypeViewModel;
    int8_t m_selectedSeries;
    int16_t m_selectedIndex;
  };

  static App* app() { return static_cast<App*>(Escher::App::app()); }

  const char* memoizedFormulaExtension() const override;

  Escher::InputViewController* inputViewController() {
    return &m_inputViewController;
  }
  Snapshot* snapshot() {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }
  const Snapshot* snapshot() const {
    return static_cast<const Snapshot*>(Escher::App::snapshot());
  }

 private:
  App(Snapshot* snapshot);
  void didBecomeActive(Escher::Window* window) override;

  // AlternateViewDelegate (choses between quantitative and categorical views)
  int activeViewControllerIndex() const override {
    return DataTypeViewModel::IndexOfDataTypeView(
        snapshot()->dataTypeViewModel()->selectedDataType());
  }
  Escher::ViewController::TitlesDisplay alternateViewTitlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::NeverDisplayOwnTitle;
  }
  void activeViewDidBecomeFirstResponder(
      Escher::ViewController* activeViewController) override {
    app()->setFirstResponder(activeViewController);
  }

  // AlternateEmptyViewDelegate (for graph tab: choses between empty or graph,
  // depending on dataType)
  bool isEmpty() const override {
    return snapshot()->dataTypeViewModel()->isCategorical()
               ? !m_categoricalStore.hasActiveGroups()
               : !m_store.hasActiveSeries();
  }
  I18n::Message emptyMessage() override { return I18n::Message::NoDataToPlot; }
  Escher::Responder* responderWhenEmpty() override {
    m_tabViewController.selectTab();
    return &m_tabViewController;
  }

  struct StoreTab : public Escher::Tab {
    StoreTab();
    constexpr static I18n::Message k_title = I18n::Message::DataTab;
    Escher::ViewController* top() override {
      return &m_storeStackViewController;
    }
    void switchActiveVariant(DataTypeViewModel::DataType type,
                             bool destroy = true);

    union ControllersVariant {
      struct {
        StoreController m_storeController;
        Escher::ButtonRowController m_storeHeader;
      } quantitative;
      struct CategoricalControllers {
        Categorical::StoreController m_storeController;
        Escher::ButtonRowController m_storeHeader;
      } categorical;
      ControllersVariant(){};
      ~ControllersVariant() {
        if (App::app()->snapshot()->dataTypeViewModel()->selectedDataType() ==
            DataTypeViewModel::DataType::Quantitative) {
          quantitative.m_storeController
              .Statistics::StoreController::~StoreController();
        }
      };
    } m_variant;
    Escher::AlternateViewController m_alternateDataTypeController;
    DataTypeController m_dataTypeController;
    Escher::StackViewController::Default m_storeStackViewController;
  };

  struct GraphTab : public Escher::Tab {
    GraphTab();
    constexpr static I18n::Message k_title = I18n::Message::StatisticsGraphTab;
    Escher::ViewController* top() override {
      return &m_graphMenuAlternateEmptyViewController;
    }
    /* TODO: We could save space by doing an union of the 2 exclusive sets of
     * controller. It's useless for now because StoreTab is bigger.
     * See [ControllersVariant] in [StoreTab] for implementation */

    // Quantitative controllers
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
    HistogramMainController m_histogramMainController;
    Escher::ButtonRowController m_histogramHeader;
    GraphTypeController m_graphTypeController;
    Escher::AlternateViewController m_graphController;
    // Categorical controllers
    Categorical::BarGraphController m_barGraphController;
    Escher::ButtonRowController m_barGraphHeader;
    Categorical::PieGraphController m_pieGraphController;
    Escher::ButtonRowController m_pieGraphHeader;
    Categorical::GraphTypeController m_categoricalGraphTypeController;
    Escher::AlternateViewController m_categoricalGraphController;
    // General controllers
    Escher::AlternateViewController m_alternateDataTypeController;
    Escher::StackViewController::Default m_graphMenuStackViewController;
    Escher::AlternateEmptyViewController
        m_graphMenuAlternateEmptyViewController;
  };

  struct CalculationTab : public Escher::Tab {
    CalculationTab();
    constexpr static I18n::Message k_title = I18n::Message::StatTab;
    Escher::ViewController* top() override { return &m_calculationHeader; }
    CalculationController m_calculationController;
    Escher::AlternateEmptyViewController
        m_calculationAlternateEmptyViewController;
    Escher::ButtonRowController
        m_calculationHeader;  // Needed for upper margin only
  };

  Store m_store;
  Categorical::Store m_categoricalStore;
  Escher::InputViewController m_inputViewController;
  Escher::TabUnion<StoreTab, GraphTab, CalculationTab> m_tabs;
  Escher::TabUnionViewController m_tabViewController;

  static_assert(sizeof(StoreTab) > sizeof(GraphTab),
                "GraphTab should be optimized. See TODO in GraphTab.");
  static_assert(sizeof(StoreTab) > sizeof(CalculationTab),
                "CalculationTab should be optimized. See TODO in GraphTab.");
};

}  // namespace Statistics
