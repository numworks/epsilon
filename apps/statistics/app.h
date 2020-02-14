#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher.h>
#include "box_controller.h"
#include "calculation_controller.h"
#include "histogram_controller.h"
#include "store.h"
#include "store_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Statistics {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
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
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  ButtonRowController m_calculationHeader;
  BoxController m_boxController;
  AlternateEmptyViewController m_boxAlternateEmptyViewController;
  ButtonRowController m_boxHeader;
  HistogramController m_histogramController;
  AlternateEmptyViewController m_histogramAlternateEmptyViewController;
  ButtonRowController m_histogramHeader;
  StackViewController m_histogramStackViewController;
  StoreController m_storeController;
  ButtonRowController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
