#include "app.h"

#include <apps/global_preferences.h>

#include <array>

#include "../apps_container.h"
#include "../shared/global_context.h"
#include "sequence_icon.h"

using namespace Poincare;
using namespace Escher;

namespace Sequence {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::SequenceApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::SequenceAppCapital;
}

const Image *App::Descriptor::icon() const { return ImageStore::SequenceIcon; }

App::Snapshot::Snapshot()
    : Shared::FunctionApp::Snapshot::Snapshot(),
      m_intervalModifiedByUser(false) {
  // Register u, v and w as reserved names to the sharedStorage.
  Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier()
      ->registerArrayOfReservedNames(
          Shared::SequenceStore::k_sequenceNames, Ion::Storage::seqExtension, 0,
          std::size(Shared::SequenceStore::k_sequenceNames));
}

App *App::Snapshot::unpack(Container *container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::resetInterval() {
  m_interval.reset();
  m_interval.parameters()->setStart(
      GlobalPreferences::sharedGlobalPreferences->sequencesInitialRank());
  m_interval.forceRecompute();
  setIntervalModifiedByUser(false);
}

void App::Snapshot::updateInterval() {
  if (!intervalModifiedByUser()) {
    int smallestInitRank = functionStore()->smallestInitialRank();
    if (smallestInitRank < Shared::Sequence::k_maxInitialRank &&
        smallestInitRank != interval()->parameters()->start()) {
      interval()->translateTo(smallestInitRank);
    }
  }
  if (interval()->isEmpty()) {
    resetInterval();
  }
}

void App::Snapshot::reset() {
  Shared::FunctionApp::Snapshot::reset();
  resetInterval();
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor *App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::tidy() {
  m_graphRange.setDelegate(nullptr);
  functionStore()->tidyDownstreamPoolFrom();
}

bool App::isAcceptableExpression(const Poincare::Expression exp) {
  /* Complete ExpressionFieldDelegateApp acceptable conditions by not accepting
   * any OperatorType. */
  return ExpressionFieldDelegateApp::isAcceptableExpression(exp) &&
         exp.type() != ExpressionNode::Type::Comparison;
}

App::ListTab::ListTab()
    : Shared::FunctionApp::ListTab(&m_listController),
      m_listController(&m_listFooter, app(), &m_listHeader, &m_listFooter) {}

App::GraphTab::GraphTab()
    : Shared::FunctionApp::GraphTab(&m_graphController),
      m_graphController(&m_graphAlternateEmptyViewController, app(),
                        &m_graphHeader, app()->snapshot()->graphRange(),
                        app()->snapshot()->cursor(),
                        app()->snapshot()->selectedCurveIndex(),
                        app()->snapshot()->functionStore()) {}

App::ValuesTab::ValuesTab()
    : Shared::FunctionApp::ValuesTab(&m_valuesController),
      m_valuesController(&m_valuesAlternateEmptyViewController, app(),
                         &m_valuesHeader) {}

}  // namespace Sequence
