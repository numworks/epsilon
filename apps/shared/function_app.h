#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include <escher/alternate_empty_view_controller.h>
#include <escher/input_view_controller.h>
#include <escher/tab_view_data_source.h>
#include "expression_field_delegate_app.h"
#include "function_graph_controller.h"
#include "function_list_controller.h"
#include "function_store.h"
#include "curve_view_cursor.h"
#include "values_controller.h"
#include "shared_app.h"

namespace Shared {

class FunctionApp : public ExpressionFieldDelegateApp {
public:
  class Snapshot : public Shared::SharedApp::Snapshot, public Escher::TabViewDataSource {
  public:
    Snapshot();
    CurveViewCursor * cursor() { return &m_cursor; }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
    virtual FunctionStore * functionStore() = 0;
    int * indexFunctionSelectedByCursor() { return &m_indexFunctionSelectedByCursor; }
    void reset() override;
  protected:
    CurveViewCursor m_cursor;
  private:
    int m_indexFunctionSelectedByCursor;
    uint32_t m_rangeVersion;
  };
  static FunctionApp * app() {
    return static_cast<FunctionApp *>(Escher::Container::activeApp());
  }
  virtual ~FunctionApp() = default;
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }
  void storageDidChangeForRecord(Ion::Storage::Record record) override;
  virtual FunctionStore * functionStore() { return snapshot()->functionStore(); }
  virtual ValuesController * valuesController() = 0;
  virtual Escher::InputViewController * inputViewController() = 0;
protected:
  FunctionApp(Snapshot * snapshot, Shared::FunctionListController * listController, Shared::FunctionGraphController * graphController, Shared::ValuesController * valuesController);

  Escher::ButtonRowController m_listFooter;
  Escher::ButtonRowController m_listHeader;
  Escher::StackViewController m_listStackViewController;
  Escher::AlternateEmptyViewController m_graphAlternateEmptyViewController;
  Escher::ButtonRowController m_graphHeader;
  Escher::StackViewController m_graphStackViewController;
  Escher::AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  Escher::ButtonRowController m_valuesHeader;
  Escher::StackViewController m_valuesStackViewController;
  Escher::TabViewController m_tabViewController;
  Escher::InputViewController m_inputViewController;
};

}

#endif
