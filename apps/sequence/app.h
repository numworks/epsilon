#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include <escher.h>
#include <poincare.h>
#include "sequence_context.h"
#include "sequence_icon.h"
#include "sequence_store.h"
#include "graph/graph_controller.h"
#include "graph/curve_view_range.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"

namespace Sequence {

class App final : public Shared::FunctionApp {
public:
  class Descriptor final : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::SequenceApp;
    }
    I18n::Message upperName() override {
      return I18n::Message::SequenceAppCapital;
    }
    const Image * icon() override {
      return ImageStore::SequenceIcon;
    }
  };
  class Snapshot final : public Shared::FunctionApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override {
      return new App(container, this);
    }
    void reset() override;
    Descriptor * descriptor() override {
      return &s_descriptor;
    }
    SequenceStore * sequenceStore() {
      return &m_sequenceStore;
    }
    CurveViewRange * graphRange() {
      return &m_graphRange;
    }
  private:
    void tidy() override;
    SequenceStore m_sequenceStore;
    CurveViewRange m_graphRange;
    static Descriptor s_descriptor;
  };
  InputViewController * inputViewController() override {
    return &m_inputViewController;
  }
  SequenceContext * localContext() override;
  const char * XNT() override;
private:
  App(Container * container, Snapshot * snapshot);
  SequenceContext m_sequenceContext;
  ListController m_listController;
  ButtonRowController m_listFooter;
  ButtonRowController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  ButtonRowController m_valuesHeader;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
