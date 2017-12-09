#ifndef STAT_APP_H
#define STAT_APP_H

#include <escher.h>
#include "box_controller.h"
#include "calculation_controller.h"
#include "store.h"
#include "store_controller.h"
#include "histogram_controller.h"
#include "../shared/text_field_delegate_app.h"
#include "stat_icon.h"
#include "../i18n.h"

namespace Statistics {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::StatsApp;
    }
    I18n::Message upperName() override {
      return I18n::Message::StatsAppCapital;
    }
    const Image * icon() override {
      return ImageStore::StatIcon;
    }
  };
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot() :
      m_store(),
      m_storeVersion(0),
      m_barVersion(0),
      m_rangeVersion(0),
      m_selectedHistogramBarIndex(0),
      m_selectedBoxQuantile(BoxView::Quantile::Min) {}
    App * unpack(Container * container) override {
      return new App(container, this);
    }
    void reset() override;
    Descriptor * descriptor() override { return &s_descriptor; }
    Store * store() { return &m_store; }
    uint32_t * storeVersion() { return &m_storeVersion; }
    uint32_t * barVersion() { return &m_barVersion; }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
    int * selectedHistogramBarIndex() { return &m_selectedHistogramBarIndex; }
    BoxView::Quantile * selectedBoxQuantile() { return &m_selectedBoxQuantile; }
  private:
    Store m_store;
    uint32_t m_storeVersion;
    uint32_t  m_barVersion;
    uint32_t m_rangeVersion;
    int m_selectedHistogramBarIndex;
    BoxView::Quantile m_selectedBoxQuantile;
    static Descriptor s_descriptor;
  };
private:
  App(Container * container, Snapshot * snapshot) :
    TextFieldDelegateApp(container, snapshot, &m_tabViewController),
    m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, snapshot->store()),
    m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
    m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
    m_boxController(&m_boxAlternateEmptyViewController, &m_boxHeader, snapshot->store(), snapshot->selectedBoxQuantile()),
    m_boxAlternateEmptyViewController(&m_boxHeader, &m_boxController, &m_boxController),
    m_boxHeader(&m_tabViewController, &m_boxAlternateEmptyViewController, &m_boxController),
    m_histogramController(&m_histogramAlternateEmptyViewController, &m_histogramHeader, snapshot->store(), snapshot->storeVersion(), snapshot->barVersion(), snapshot->rangeVersion(), snapshot->selectedHistogramBarIndex()),
    m_histogramAlternateEmptyViewController(&m_histogramHeader, &m_histogramController, &m_histogramController),
    m_histogramHeader(&m_histogramStackViewController, &m_histogramAlternateEmptyViewController, &m_histogramController),
    m_histogramStackViewController(&m_tabViewController, &m_histogramHeader),
    m_storeController(&m_storeHeader, snapshot->store(), &m_storeHeader),
    m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
    m_storeStackViewController(&m_tabViewController, &m_storeHeader),
    m_tabViewController(&m_modalViewController, snapshot, &m_storeStackViewController, &m_histogramStackViewController, &m_boxHeader, &m_calculationHeader)
  {
  }
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
