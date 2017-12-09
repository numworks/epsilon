#ifndef REGRESSION_APP_H
#define REGRESSION_APP_H

#include <escher.h>
#include "regression_icon.h"
#include "../shared/text_field_delegate_app.h"
#include "store.h"
#include "store_controller.h"
#include "graph_controller.h"
#include "calculation_controller.h"

namespace Regression {

class App final : public Shared::TextFieldDelegateApp {
public:
  class Descriptor final : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::RegressionApp;
    }
    I18n::Message upperName() override {
      return I18n::Message::RegressionAppCapital;
    }
    const Image * icon() override {
      return ImageStore::RegressionIcon;
    }
  };
  class Snapshot final : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot() : m_store(), m_cursor(), m_graphSelectedDotIndex(-1), m_modelVersion(0), m_rangeVersion(0) {}
    App * unpack(Container * container) override {
      return new App(container, this);
    }
    void reset() override;
    Descriptor * descriptor() override {
      return &s_descriptor;
    }
    Store * store() {
      return &m_store;
    }
    Shared::CurveViewCursor * cursor() {
      return &m_cursor;
    }
    int * graphSelectedDotIndex() {
      return &m_graphSelectedDotIndex;
    }
    uint32_t * modelVersion() {
      return &m_modelVersion;
    }
    uint32_t * rangeVersion() {
      return &m_rangeVersion;
    }
  private:
    Store m_store;
    Shared::CurveViewCursor m_cursor;
    int m_graphSelectedDotIndex;
    uint32_t m_modelVersion;
    uint32_t m_rangeVersion;
    static Descriptor s_descriptor;
 };
private:
  App(Container * container, Snapshot * snapshot);
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  ButtonRowController m_calculationHeader;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  StoreController m_storeController;
  ButtonRowController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
