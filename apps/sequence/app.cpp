#include "app.h"
#include "../apps_container.h"
#include "sequence_icon.h"
#include "../shared/global_context.h"

using namespace Poincare;

namespace Sequence {

I18n::Message App::Descriptor::name() {
  return I18n::Message::SequenceApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::SequenceAppCapital;
}

App::Descriptor::ExaminationLevel App::Descriptor::examinationLevel() {
  return App::Descriptor::ExaminationLevel::Strict;
}

const Image * App::Descriptor::icon() {
  return ImageStore::SequenceIcon;
}

App::Snapshot::Snapshot() :
  Shared::FunctionApp::Snapshot::Snapshot(),
  m_graphRange()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  Shared::FunctionApp::Snapshot::reset();
  m_interval.reset();
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::tidy() {
  m_graphRange.setDelegate(nullptr);
}

App::App(Snapshot * snapshot) :
  FunctionApp(snapshot, &m_inputViewController),
  m_sequenceContext(AppsContainer::sharedAppsContainer()->globalContext(), static_cast<Shared::GlobalContext *>(AppsContainer::sharedAppsContainer()->globalContext())->sequenceStore()),
  m_listController(&m_listFooter, this, &m_listHeader, &m_listFooter),
  m_listFooter(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray),
  m_listHeader(nullptr, &m_listFooter, &m_listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader),
  m_graphController(&m_graphAlternateEmptyViewController, this, snapshot->functionStore(), snapshot->graphRange(), snapshot->cursor(), snapshot->indexFunctionSelectedByCursor(), snapshot->rangeVersion(), &m_graphHeader),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, this, &m_valuesHeader),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController),
  m_valuesHeader(nullptr, &m_valuesAlternateEmptyViewController, &m_valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader),
  m_tabViewController(&m_inputViewController, snapshot, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, &m_listController, &m_listController, &m_listController)
{
}

Shared::SequenceContext * App::localContext() {
  return &m_sequenceContext;
}

NestedMenuController * App::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  MathVariableBoxController * varBox = AppsContainer::sharedAppsContainer()->variableBoxController();
  varBox->setSender(textInput);
  varBox->lockDeleteEvent(MathVariableBoxController::Page::Sequence);
  return varBox;
}

}
