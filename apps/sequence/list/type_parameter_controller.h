#ifndef SEQUENCE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMATER_CONTROLLER_H

#include <escher/even_odd_expression_cell.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/scrollable_expression_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <poincare/layout.h>

#include "../../shared/sequence_store.h"

namespace Sequence {

class ListController;

constexpr int k_numberOfCells = 3;

class TypeParameterController
    : public Escher::SelectableCellListPage<
          Escher::MenuCell<Escher::ScrollableExpressionView,
                           Escher::MessageTextView>,
          k_numberOfCells, Escher::RegularListViewDataSource> {
 public:
  TypeParameterController(Escher::Responder* parentResponder,
                          ListController* list, KDCoordinate topMargin = 0,
                          KDCoordinate rightMargin = 0,
                          KDCoordinate bottomMargin = 0,
                          KDCoordinate leftMargin = 0);
  const char* title() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int j) override;
  void setRecord(Ion::Storage::Record record);

 private:
  constexpr static int k_indexOfExplicit =
      static_cast<int>(Shared::Sequence::Type::Explicit);
  constexpr static int k_indexOfRecurrence =
      static_cast<int>(Shared::Sequence::Type::SingleRecurrence);
  constexpr static int k_indexOfDoubleRecurrence =
      static_cast<int>(Shared::Sequence::Type::DoubleRecurrence);

  Escher::StackViewController* stackController() const;
  Shared::Sequence* sequence() {
    assert(!m_record.isNull());
    return sequenceStore()->modelForRecord(m_record);
  }
  Shared::SequenceStore* sequenceStore();
  Poincare::Layout m_layouts[k_numberOfCells];
  Ion::Storage::Record m_record;
  ListController* m_listController;
};

}  // namespace Sequence

#endif
