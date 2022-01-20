#include "type_parameter_controller.h"
#include "list_controller.h"
#include "../app.h"
#include <assert.h>
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <apps/i18n.h>

using namespace Poincare;
using namespace Shared;

namespace Sequence {

TypeParameterController::TypeParameterController(Responder * parentResponder, ListController * list, TableCell::Layout cellLayout,
  KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ViewController(parentResponder),
  m_explicitCell(&m_selectableTableView, I18n::Message::Explicit, cellLayout),
  m_singleRecurrenceCell(&m_selectableTableView, I18n::Message::SingleRecurrence, cellLayout),
  m_doubleRecurrenceCell(&m_selectableTableView, I18n::Message::DoubleRecurrence, cellLayout),
  m_layouts{},
  m_selectableTableView(this),
  m_record(),
  m_listController(list)
{
  m_selectableTableView.setMargins(topMargin, rightMargin, bottomMargin, leftMargin);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

const char * TypeParameterController::title() {
  if (m_record.isNull()) {
    return I18n::translate(I18n::Message::ChooseSequenceType);
  }
  return I18n::translate(I18n::Message::SequenceType);
}

View * TypeParameterController::view() {
  return &m_selectableTableView;
}

void TypeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

void TypeParameterController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

void TypeParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, m_record == nullptr ? 0 : static_cast<uint8_t>(sequence()->type()));
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (!m_record.isNull()) {
      Shared::Sequence::Type sequenceType = (Shared::Sequence::Type)selectedRow();
      if (sequence()->type() != sequenceType) {
        m_listController->selectPreviousNewSequenceCell();
        sequence()->setType(sequenceType);
        // Invalidate sequence context cache when changing sequence type
        App::app()->localContext()->resetCache();
        // Reset the first index if the new type is "Explicit"
        if (sequenceType == Shared::Sequence::Type::Explicit) {
          sequence()->setInitialRank(0);
        }
      }
      StackViewController * stack = stackController();
      assert(stack->depth()>2);
      stack->pop();
      stack->pop();
      return true;
    }

    Ion::Storage::Record::ErrorStatus error = sequenceStore()->addEmptyModel();
    if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
      return false;
    }
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    Ion::Storage::Record record = sequenceStore()->recordAtIndex(sequenceStore()->numberOfModels()-1);
    Shared::Sequence * newSequence = sequenceStore()->modelForRecord(record);
    newSequence->setType((Shared::Sequence::Type)selectedRow());
    Container::activeApp()->dismissModalViewController();
    m_listController->editExpression(0, Ion::Events::OK);
    return true;
  }
  if (event == Ion::Events::Left && !m_record.isNull()) {
    stackController()->pop();
    return true;
  }
  return false;
}

int TypeParameterController::numberOfRows() const {
  return k_totalNumberOfCell;
};

HighlightCell * TypeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_explicitCell, &m_singleRecurrenceCell, &m_doubleRecurrenceCell};
  return cells[index];
}

int TypeParameterController::reusableCellCount() const {
  return k_totalNumberOfCell;
}

KDCoordinate TypeParameterController::cellHeight() {
  if (m_record.isNull()) {
    return 50;
  }
  return Metric::ParameterCellHeight;
}

void TypeParameterController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  const char * nextName = sequenceStore()->firstAvailableName();
  const KDFont * font = KDFont::LargeFont;
  if (!m_record.isNull()) {
    nextName = sequence()->fullName();
    font = KDFont::SmallFont;
  }
  const char * subscripts[3] = {"n", "n+1", "n+2"};
  m_layouts[j] = HorizontalLayout::Builder(
        CodePointLayout::Builder(nextName[0], font),
        VerticalOffsetLayout::Builder(LayoutHelper::String(subscripts[j], strlen(subscripts[j]), font), VerticalOffsetLayoutNode::Position::Subscript)
      );
  ExpressionTableCellWithPointer * myCell = (ExpressionTableCellWithPointer *)cell;
  myCell->setLayout(m_layouts[j]);
}

void TypeParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

SequenceStore * TypeParameterController::sequenceStore() {
  return App::app()->functionStore();
}

StackViewController * TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
