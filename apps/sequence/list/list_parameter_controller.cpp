#include "list_parameter_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/print.h>

#include "../app.h"
#include "list_controller.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Sequence {

ListParameterController::ListParameterController(ListController* listController)
    : Shared::ListParameterController(listController,
                                      I18n::Message::SequenceColor,
                                      I18n::Message::DeleteSequence, this),
      m_firstRankCell(&m_selectableListView, this),
      m_typeParameterController(this, listController, Metric::CommonMargins) {
  m_typeCell.label()->setMessage(I18n::Message::SequenceType);
  m_firstRankCell.label()->setMessage(I18n::Message::FirstTermIndex);
}

const char* ListParameterController::title() {
  return I18n::translate(I18n::Message::SequenceOptions);
}

bool ListParameterController::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return event == Ion::Events::Down || event == Ion::Events::Up ||
         MathTextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ListParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  int index = std::floor(floatBody);
  if (index < 0 || index > Shared::Sequence::k_maxInitialRank) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  sequence()->setInitialRank(index);
  updateFirstRankCell();

  App::app()->snapshot()->updateInterval();
  // Invalidate sequence context cache when changing sequence type
  App::app()->localContext()->resetCache();
  m_selectableListView.handleEvent(event);
  return true;
}

void ListParameterController::listViewDidChangeSelectionAndDidScroll(
    SelectableListView* l, int previousSelectedRow, KDPoint previousOffset,
    bool withinTemporarySelection) {
  assert(l == &m_selectableListView);
  if (withinTemporarySelection || previousSelectedRow == l->selectedRow()) {
    return;
  }
  if (previousSelectedRow == 1) {
    assert(l->cell(previousSelectedRow) == &m_firstRankCell);
    m_firstRankCell.textField()->setEditing(false);
    App::app()->setFirstResponder(&m_selectableListView);
  }
  if (l->selectedRow() == 1) {
    assert(l->selectedCell() == &m_firstRankCell);
    App::app()->setFirstResponder(&m_firstRankCell);
  }
}

HighlightCell* ListParameterController::cell(int row) {
  assert(0 <= row && row < numberOfRows());
  HighlightCell* const cells[] = {&m_typeCell, &m_firstRankCell, &m_enableCell,
                                  &m_colorCell, &m_deleteCell};
  return cells[row];
}

void ListParameterController::viewWillAppear() {
  if (!m_record.isNull()) {
    m_typeCell.subLabel()->setLayout(sequence()->definitionName());
    updateFirstRankCell();
  }
  Shared::ListParameterController::viewWillAppear();
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  if (cell == &m_typeCell && m_typeCell.canBeActivatedByEvent(event)) {
    m_typeParameterController.setRecord(m_record);
    static_cast<StackViewController*>(parentResponder())
        ->push(&m_typeParameterController);
    return true;
  }
  if (cell == &m_enableCell && m_enableCell.canBeActivatedByEvent(event)) {
    App::app()->localContext()->resetCache();
  }
  return Shared::ListParameterController::handleEvent(event);
}

void ListParameterController::updateFirstRankCell() {
  char buffer[Shared::Sequence::k_initialRankNumberOfDigits + 1];
  OMG::Print::IntLeft(sequence()->initialRank(), buffer,
                      Shared::Sequence::k_initialRankNumberOfDigits + 1);
  m_firstRankCell.textField()->setText(buffer);
}

}  // namespace Sequence
