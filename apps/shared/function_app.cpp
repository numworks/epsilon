#include "function_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

FunctionApp::Snapshot::Snapshot() :
  m_indexFunctionSelectedByCursor(0),
  m_rangeVersion(0)
{
}

void FunctionApp::Snapshot::reset() {
  m_indexFunctionSelectedByCursor = 0;
  m_rangeVersion = 0;
  setActiveTab(0);
}

void FunctionApp::storageDidChangeForRecord(Ion::Storage::Record record) {
  functionStore()->storageDidChangeForRecord(record);
}

FunctionApp::FunctionApp(Snapshot * snapshot, Shared::FunctionListController * listController, Shared::FunctionGraphController * graphController, Shared::ValuesController * valuesController) :
  ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
  m_listFooter(&m_listHeader, listController, listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray),
  m_listHeader(&m_listStackViewController, &m_listFooter, listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader, Escher::StackViewController::Style::WhiteUniform),
  m_graphAlternateEmptyViewController(&m_graphHeader, graphController, graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader, Escher::StackViewController::Style::WhiteUniform),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, valuesController, valuesController),
  m_valuesHeader(&m_valuesStackViewController, &m_valuesAlternateEmptyViewController, valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader, Escher::StackViewController::Style::WhiteUniform),
  m_tabViewController(&m_inputViewController, snapshot, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, listController, listController, listController)
{
}

}
