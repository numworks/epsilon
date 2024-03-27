#include "type_parameter_controller.h"

#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../app.h"
#include "list_controller.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Sequence {

TypeParameterController::TypeParameterController(Responder *parentResponder,
                                                 ListController *list,
                                                 KDMargins margins)
    : UniformSelectableListController<MenuCell<LayoutView, MessageTextView>,
                                      k_numberOfCells>(parentResponder),
      m_listController(list) {
  cell(k_indexOfExplicit)->subLabel()->setMessage(I18n::Message::Explicit);
  cell(k_indexOfRecurrence)
      ->subLabel()
      ->setMessage(I18n::Message::SingleRecurrence);
  cell(k_indexOfDoubleRecurrence)
      ->subLabel()
      ->setMessage(I18n::Message::DoubleRecurrence);
  m_selectableListView.setMargins(margins);
  m_selectableListView.hideScrollBars();
}

const char *TypeParameterController::title() {
  return I18n::translate(isNewModel() ? I18n::Message::ChooseSequenceType
                                      : I18n::Message::SequenceType);
}

void TypeParameterController::viewWillAppear() {
  const char *nextName = isNewModel() ? SequenceStore::FirstAvailableName()
                                      : sequence()->fullName();
  assert(nextName != nullptr);
  const char *subscripts[k_numberOfCells] = {"n", "n+1", "n+2"};
  for (size_t j = 0; j < k_numberOfCells; j++) {
    cell(j)->label()->setLayout(Layout::Create(
        KA ^ KSubscriptL(KB), {.KA = Layout::CodePoint(nextName[0]),
                               .KB = Layout::String(subscripts[j])}));
  }
  ViewController::viewWillAppear();
  m_selectableListView.reloadData();
}

void TypeParameterController::viewDidDisappear() {
  // Tidy layouts
  for (size_t i = 0; i < k_numberOfCells; i++) {
    cell(i)->label()->setLayout(Layout());
  }
  m_selectableListView.deselectTable();
  ViewController::viewDidDisappear();
}

void TypeParameterController::didBecomeFirstResponder() {
  selectRow(isNewModel() ? k_indexOfExplicit
                         : static_cast<uint8_t>(sequence()->type()));
  SelectableListViewController::didBecomeFirstResponder();
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (!isNewModel()) {
      Shared::Sequence::Type sequenceType =
          static_cast<Shared::Sequence::Type>(selectedRow());
      if (sequence()->type() != sequenceType) {
        m_listController->selectPreviousNewSequenceCell();
        sequence()->setType(sequenceType);
        // Invalidate sequence context cache when changing sequence type
        App::app()->localContext()->resetCache();
        // Reset the first index if the new type is "Explicit"
        if (sequenceType == Shared::Sequence::Type::Explicit) {
          sequence()->setInitialRank(
              GlobalPreferences::SharedGlobalPreferences()
                  ->sequencesInitialRank());
        }
      }
      StackViewController *stack = stackController();
      assert(stack->depth() > 2);
      stack->pop();
      stack->pop();
      return true;
    }

    Ion::Storage::Record::ErrorStatus error = sequenceStore()->addEmptyModel();
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return true;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    Ion::Storage::Record record =
        sequenceStore()->recordAtIndex(sequenceStore()->numberOfModels() - 1);
    Shared::Sequence *newSequence = sequenceStore()->modelForRecord(record);
    newSequence->setType(static_cast<Shared::Sequence::Type>(selectedRow()));
    // Make all the lines of the added sequence visible
    m_listController->showLastSequence();
    App::app()->modalViewController()->dismissModal();
    m_listController->editExpression(Ion::Events::OK);
    return true;
  }
  if (event == Ion::Events::Left && !isNewModel()) {
    stackController()->pop();
    return true;
  }
  if (isNewModel() &&
      m_listController->handleEventOnExpressionInTemplateMenu(event)) {
    return true;
  }
  return false;
}

SequenceStore *TypeParameterController::sequenceStore() {
  return App::app()->functionStore();
}

}  // namespace Sequence
