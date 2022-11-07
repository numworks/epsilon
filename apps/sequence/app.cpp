#include "app.h"
#include "../apps_container.h"
#include "sequence_icon.h"
#include "../shared/global_context.h"

using namespace Poincare;
using namespace Escher;

namespace Sequence {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::SequenceApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::SequenceAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::SequenceIcon;
}

App::Snapshot::Snapshot() : Shared::FunctionApp::Snapshot::Snapshot() {
  // Register u, v and w as reserved names to the sharedStorage.
  Ion::Storage::FileSystem::sharedFileSystem()->recordNameVerifier()->registerArrayOfReservedNames(Shared::SequenceStore::k_sequenceNames, Ion::Storage::seqExtension, 0, sizeof(Shared::SequenceStore::k_sequenceNames) / sizeof(char *));
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  Shared::FunctionApp::Snapshot::reset();
  m_interval.reset();
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::tidy() {
  m_graphRange.setDelegate(nullptr);
  functionStore()->tidyDownstreamPoolFrom();
}

bool App::isAcceptableExpression(const Poincare::Expression exp) {
  /* Complete ExpressionFieldDelegateApp acceptable conditions by not accepting
   * any OperatorType. */
  return ExpressionFieldDelegateApp::isAcceptableExpression(exp) && exp.type() != ExpressionNode::Type::Comparison;
}

bool App::storageWillChangeForRecord(Ion::Storage::Record record) {
  return !(isVarBoxMenuOpen() || isStoreMenuOpen()) || !record.hasExtension(Ion::Storage::seqExtension);
}

App::App(Snapshot * snapshot) :
  FunctionApp(snapshot, &m_listController, &m_graphController, &m_valuesController),
  m_listController(&m_listFooter, this, &m_listHeader, &m_listFooter),
  m_graphController(&m_graphAlternateEmptyViewController, this, snapshot->functionStore(), snapshot->graphRange(), snapshot->cursor(), snapshot->indexFunctionSelectedByCursor(), snapshot->rangeVersion(), &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, this, &m_valuesHeader)
{
}

}
