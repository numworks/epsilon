#include "type_parameter_controller.h"

#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>

#include "../app.h"
#include "list_controller.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Sequence {

TypeParameterController::TypeParameterController(Responder *parentResponder,
                                                 ListController *list,
                                                 KDCoordinate topMargin,
                                                 KDCoordinate rightMargin,
                                                 KDCoordinate bottomMargin,
                                                 KDCoordinate leftMargin)
    : SelectableCellListPage<
          MenuCell<ScrollableExpressionView, MessageTextView>, k_numberOfCells,
          RegularListViewDataSource>(parentResponder),
      m_listController(list) {
  for (int i = 0; i < k_numberOfCells; i++) {
    cellAtIndex(i)->label()->setParentResponder(&m_selectableListView);
  }
  cellAtIndex(k_indexOfExplicit)
      ->subLabel()
      ->setMessage(I18n::Message::Explicit);
  cellAtIndex(k_indexOfRecurrence)
      ->subLabel()
      ->setMessage(I18n::Message::SingleRecurrence);
  cellAtIndex(k_indexOfDoubleRecurrence)
      ->subLabel()
      ->setMessage(I18n::Message::DoubleRecurrence);
  m_selectableListView.setMargins(topMargin, rightMargin, bottomMargin,
                                  leftMargin);
  m_selectableListView.hideScrollBars();
}

const char *TypeParameterController::title() {
  if (m_record.isNull()) {
    return I18n::translate(I18n::Message::ChooseSequenceType);
  }
  return I18n::translate(I18n::Message::SequenceType);
}

void TypeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableListView.reloadData();
}

void TypeParameterController::viewDidDisappear() {
  m_selectableListView.deselectTable();
  ViewController::viewDidDisappear();
}

void TypeParameterController::didBecomeFirstResponder() {
  selectCell(m_record == nullptr ? 0
                                 : static_cast<uint8_t>(sequence()->type()));
  SelectableListViewController::didBecomeFirstResponder();
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (!m_record.isNull()) {
      Shared::Sequence::Type sequenceType =
          static_cast<Shared::Sequence::Type>(selectedRow());
      if (sequence()->type() != sequenceType) {
        m_listController->selectPreviousNewSequenceCell();
        sequence()->setType(sequenceType);
        // Invalidate sequence context cache when changing sequence type
        App::app()->localContext()->resetCache();
        // Reset the first index if the new type is "Explicit"
        if (sequenceType == Shared::Sequence::Type::Explicit) {
          sequence()->setInitialRank(GlobalPreferences::sharedGlobalPreferences
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
    newSequence->setInitialRank(
        GlobalPreferences::sharedGlobalPreferences->sequencesInitialRank());
    newSequence->setType(static_cast<Shared::Sequence::Type>(selectedRow()));
    // Make all the lines of the added sequence visible
    m_listController->showLastSequence();
    Container::activeApp()->modalViewController()->dismissModal();
    m_listController->editExpression(Ion::Events::OK);
    return true;
  }
  if (event == Ion::Events::Left && !m_record.isNull()) {
    stackController()->pop();
    return true;
  }
  if (m_record.isNull() &&
      m_listController->handleEventOnExpressionInTemplateMenu(event)) {
    return true;
  }
  return false;
}

void TypeParameterController::willDisplayCellForIndex(HighlightCell *cell,
                                                      int j) {
  const char *nextName = sequenceStore()->firstAvailableName();
  KDFont::Size font = KDFont::Size::Large;
  if (!m_record.isNull()) {
    nextName = sequence()->fullName();
    font = KDFont::Size::Small;
  }
  if (nextName == nullptr) {
    /* When unselecting this controller, rowHeight and willDisplayCellForIndex
     * might be called with a null record while the sequence still exists.
     * NextName is then the next available name which is nullptr when dealing
     * with last sequence. As it won't be actually displayed, we use a default
     * placeholder. */
    nextName = "?";
  }
  const char *subscripts[3] = {"n", "n+1", "n+2"};
  m_layouts[j] = HorizontalLayout::Builder(
      CodePointLayout::Builder(nextName[0]),
      VerticalOffsetLayout::Builder(
          LayoutHelper::String(subscripts[j], strlen(subscripts[j])),
          VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  MenuCell<ScrollableExpressionView, MessageTextView> *myCell =
      static_cast<MenuCell<ScrollableExpressionView, MessageTextView> *>(cell);
  myCell->label()->setLayout(m_layouts[j]);
  myCell->label()->setFont(font);
}

void TypeParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

SequenceStore *TypeParameterController::sequenceStore() {
  return App::app()->functionStore();
}

StackViewController *TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}

}  // namespace Sequence
