#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher/alternate_empty_view_controller.h>
#include <escher/alternate_view_controller.h>
#include <escher/tab_view_data_source.h>
#include <escher/tab_view_controller.h>
#include <apps/shared/text_field_delegate_app.h>
#include <apps/shared/shared_app.h>
#include "graph/box_controller.h"
#include "graph/frequency_controller.h"
#include "graph/histogram_controller.h"
#include "graph/graph_type_controller.h"
#include "graph/graph_view_model.h"
#include "graph/normal_probability_controller.h"
#include "stats/calculation_controller.h"
#include "data/store_controller.h"
#include "store.h"

namespace Statistics {

class App : public Shared::TextFieldDelegateApp, Escher::AlternateViewDelegate {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot, public Escher::TabViewDataSource {
  public:
    Snapshot();
    App * unpack(Escher::Container * container) override;
    void reset() override;
    const Descriptor * descriptor() const override;
    Store * store() { return &m_store; }
    uint32_t * storeVersion() { return &m_storeVersion; }
    uint32_t * barVersion() { return &m_barVersion; }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
    GraphViewModel * graphViewModel() { return &m_graphViewModel; }
    int * selectedHistogramSeriesIndex() { return &m_selectedHistogramSeriesIndex; }
    int * selectedHistogramBarIndex() { return &m_selectedHistogramBarIndex; }
    int * selectedBoxSeriesIndex() { return &m_selectedBoxSeriesIndex; }
    BoxView::Quantile * selectedBoxQuantile() { return &m_selectedBoxQuantile; }
  private:
    Store m_store;
    uint32_t m_storeVersion;
    uint32_t  m_barVersion;
    uint32_t m_rangeVersion;
    // TODO : Store more data in m_graphViewModel
    GraphViewModel m_graphViewModel;
    int m_selectedHistogramSeriesIndex;
    int m_selectedHistogramBarIndex;
    int m_selectedBoxSeriesIndex;
    BoxView::Quantile m_selectedBoxQuantile;
  };
  TELEMETRY_ID("Statistics");
private:
  App(Snapshot * snapshot, Poincare::Context * parentContext);
  int activeViewControllerIndex() const override { return GraphViewModel::IndexOfGraphView(snapshot()->graphViewModel()->selectedGraphView()); }
  /* Alternate view title is used for the graph tab title, but shouldn't be
   * visible elsewhere. */
  const char * alternateViewTitle() override { return I18n::translate(I18n::Message::GraphTab); }
  Escher::ViewController::TitlesDisplay alternateViewTitlesDisplay() override { return Escher::ViewController::TitlesDisplay::NeverDisplayOwnTitle; }
  Snapshot * snapshot() const { return static_cast<Snapshot *>(Escher::App::snapshot()); }
  void didBecomeActive(Escher::Window * window) override;
  CalculationController m_calculationController;
  Escher::AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  Escher::ButtonRowController m_calculationHeader; // Needed for upper margin only
  NormalProbabilityController m_normalProbabilityController;
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
  Escher::AlternateEmptyViewController m_graphMenuAlternateEmptyViewController;
  StoreController m_storeController;
  Escher::StackViewController m_storeStackViewController;
  Escher::ButtonRowController m_storeHeader; // Needed for upper margin only
  Escher::TabViewController m_tabViewController;
};

}

#endif
