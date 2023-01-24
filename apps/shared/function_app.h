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
    virtual FunctionStore * functionStore() = 0;
    int * selectedCurveIndex() { return &m_selectedCurveIndex; }
    void reset() override;
  protected:
    CurveViewCursor m_cursor;
  private:
    int m_selectedCurveIndex;
  };
  static FunctionApp * app() {
    return static_cast<FunctionApp *>(Escher::Container::activeApp());
  }
  virtual ~FunctionApp() = default;
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }

  virtual FunctionStore * functionStore() const { return snapshot()->functionStore(); }
  virtual ValuesController * valuesController() = 0;
  virtual Escher::InputViewController * inputViewController() = 0;

  bool storageCanChangeForRecordName(const Ion::Storage::Record::Name recordName) const override;
  void prepareForIntrusiveStorageChange() override;
  void concludeIntrusiveStorageChange() override;

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
  Escher::ViewController * m_activeControllerBeforeStore;
};

}

#endif
