#include "app.h"

#include <apps/apps_container.h>
#include <apps/i18n.h>

#include "graph_icon.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Graph {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::FunctionApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::FunctionAppCapital;
}

const Image* App::Descriptor::icon() const { return ImageStore::GraphIcon; }

App* App::Snapshot::unpack(Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  Shared::FunctionApp::Snapshot::reset();
  for (size_t i = 0; i < ContinuousFunctionProperties::k_numberOfSymbolTypes;
       i++) {
    m_interval[i].reset();
  }
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::tidy() {
  functionStore()->setCachesContainer(nullptr);
  SharedApp::Snapshot::tidy();
}

App::ListTab::ListTab()
    : Shared::FunctionApp::ListTab(&m_listController),
      m_listController(&m_listFooter, &m_listHeader, &m_listFooter,
                       &app()->m_functionParameterController) {}

App::GraphTab::GraphTab()
    : Shared::FunctionApp::GraphTab(&m_graphController),
      m_graphController(&m_graphAlternateEmptyViewController, &m_graphHeader,
                        app()->snapshot()->graphRange(),
                        app()->snapshot()->cursor(),
                        app()->snapshot()->selectedCurveIndex()) {}

App::ValuesTab::ValuesTab()
    : Shared::FunctionApp::ValuesTab(&m_valuesController),
      m_valuesController(&m_valuesAlternateEmptyViewController, &m_valuesHeader,
                         &app()->m_functionParameterController) {}

App::App(Snapshot* snapshot)
    : FunctionApp(snapshot, &m_tabs, ListTab::k_title),
      m_functionParameterController(this, I18n::Message::FunctionColor,
                                    I18n::Message::DeleteExpression) {
  snapshot->functionStore()->setCachesContainer(&m_cachesContainer);
}

}  // namespace Graph
