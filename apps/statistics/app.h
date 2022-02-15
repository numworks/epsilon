#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher/alternate_empty_view_controller.h>
#include <escher/tab_view_data_source.h>
#include <escher/tab_view_controller.h>
#include <apps/shared/text_field_delegate_app.h>
#include <apps/shared/shared_app.h>
#include "graph/box_controller.h"
#include "stats/calculation_controller.h"
#include "graph/histogram_controller.h"
#include "data/store_controller.h"
#include "store.h"

namespace Statistics {

class App : public Shared::TextFieldDelegateApp {
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
    int * selectedHistogramSeriesIndex() { return &m_selectedHistogramSeriesIndex; }
    int * selectedHistogramBarIndex() { return &m_selectedHistogramBarIndex; }
    int * selectedBoxSeriesIndex() { return &m_selectedBoxSeriesIndex; }
    BoxView::Quantile * selectedBoxQuantile() { return &m_selectedBoxQuantile; }
  private:
    Store m_store;
    uint32_t m_storeVersion;
    uint32_t  m_barVersion;
    uint32_t m_rangeVersion;
    int m_selectedHistogramSeriesIndex;
    int m_selectedHistogramBarIndex;
    int m_selectedBoxSeriesIndex;
    BoxView::Quantile m_selectedBoxQuantile;
  };
  TELEMETRY_ID("Statistics");
private:
  App(Snapshot * snapshot, Poincare::Context * parentContext);
  CalculationController m_calculationController;
  Escher::AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  Escher::ButtonRowController m_calculationHeader; // Needed for upper margin only
  BoxController m_boxController;
  Escher::AlternateEmptyViewController m_boxAlternateEmptyViewController;
  Escher::ButtonRowController m_boxHeader;
  HistogramController m_histogramController;
  Escher::AlternateEmptyViewController m_histogramAlternateEmptyViewController;
  Escher::ButtonRowController m_histogramHeader;
  Escher::StackViewController m_histogramStackViewController;
  StoreController m_storeController;
  Escher::StackViewController m_storeStackViewController;
  Escher::ButtonRowController m_storeHeader; // Needed for upper margin only
  Escher::TabViewController m_tabViewController;
};

}

#endif
