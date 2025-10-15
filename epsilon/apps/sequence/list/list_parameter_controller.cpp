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
      m_displayNotationCell(false),
      m_notationParameterController(nullptr) {
  m_firstRankCell.label()->setMessage(I18n::Message::FirstTermIndex);
  m_notationCell.label()->setMessage(I18n::Message::SequenceNotation);
}

const char* ListParameterController::title() const {
  return I18n::translate(I18n::Message::Options);
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
  // Invalidate sequence context cache when changing initial rank
  App::app()->localContext().resetCache();
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
  if (previousSelectedRow == k_indexOfFirstRankCell) {
    assert(l->cell(previousSelectedRow) == &m_firstRankCell);
    m_firstRankCell.textField()->setEditing(false);
    App::app()->setFirstResponder(&m_selectableListView);
  }
  if (l->selectedRow() == k_indexOfFirstRankCell) {
    assert(l->selectedCell() == &m_firstRankCell);
    App::app()->setFirstResponder(&m_firstRankCell);
  }
}

const HighlightCell* ListParameterController::cell(int row) const {
  assert(0 <= row && row < numberOfRows());
  if (displayNotationCell()) {
    const HighlightCell* const cellsWithNotation[] = {
        &m_colorCell, &m_firstRankCell, &m_notationCell, &m_enableCell,
        &m_deleteCell};
    return cellsWithNotation[row];
  } else {
    const HighlightCell* const cellsWithoutNotation[] = {
        &m_colorCell, &m_firstRankCell, &m_enableCell, &m_deleteCell};
    return cellsWithoutNotation[row];
  }
}

void ListParameterController::viewWillAppear() {
  if (!m_record.isNull()) {
    updateFirstRankCell();
    updateDisplayNotationCell();
    m_notationCell.subLabel()->setLayout(sequence()->definitionName());
  }
  Shared::ListParameterController::viewWillAppear();
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (cell == &m_enableCell && m_enableCell.canBeActivatedByEvent(event)) {
    App::app()->localContext().resetCache();
  }
  if (cell == &m_notationCell && m_notationCell.canBeActivatedByEvent(event)) {
    m_notationParameterController.setRecord(m_record);
    stack->push(&m_notationParameterController);
    return true;
  }
  return Shared::ListParameterController::handleEvent(event);
}

void ListParameterController::updateFirstRankCell() {
  char buffer[Shared::Sequence::k_initialRankNumberOfDigits + 1];
  int size =
      OMG::Print::IntLeft(sequence()->initialRank(), buffer,
                          Shared::Sequence::k_initialRankNumberOfDigits + 1);
  buffer[size] = 0;
  m_firstRankCell.textField()->setText(buffer);
}

void ListParameterController::updateDisplayNotationCell() {
  m_displayNotationCell =
      sequence()->type() != Shared::Sequence::Type::Explicit;
}

}  // namespace Sequence
