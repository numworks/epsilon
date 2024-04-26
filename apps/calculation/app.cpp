#include "app.h"

#include <apps/i18n.h>
#include <poincare/old/symbol.h>

#include "calculation_icon.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Calculation {

I18n::Message App::Descriptor::name() const { return I18n::Message::CalculApp; }

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::CalculAppCapital;
}

const Image *App::Descriptor::icon() const {
  return ImageStore::CalculationIcon;
}

App *App::Snapshot::unpack(Container *container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  m_calculationStore.deleteAll();
  m_cacheBuffer[0] = 0;
  m_cacheBufferInformation = 0;
  SharedApp::Snapshot::reset();
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor *App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::Snapshot::Snapshot()
    : m_calculationStore(m_calculationBuffer, k_calculationBufferSize) {}

App::App(Snapshot *snapshot)
    : MathApp(snapshot, &m_editExpressionController),
      m_historyController(&m_editExpressionController,
                          snapshot->calculationStore()),
      m_editExpressionController(&m_modalViewController, &m_historyController,
                                 snapshot->calculationStore()) {}

void App::didBecomeActive(Window *window) {
  m_editExpressionController.restoreInput();
  m_historyController.recomputeHistoryCellHeightsIfNeeded();
  Shared::SharedApp::didBecomeActive(window);
}

}  // namespace Calculation
