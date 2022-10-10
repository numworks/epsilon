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

void FunctionApp::storageDidChangeForRecord(const Ion::Storage::Record record) {
  functionStore()->storageDidChangeForRecord(record);
}

}
