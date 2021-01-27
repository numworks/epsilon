#ifndef SEQUENCE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMATER_CONTROLLER_H

#include <escher/stack_view_controller.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/expression_table_cell_with_pointer.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/simple_list_view_data_source.h>
#include <poincare/layout.h>
#include "../../shared/sequence_store.h"

namespace Sequence {

class ListController;

class TypeParameterController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  TypeParameterController(Escher::Responder * parentResponder, ListController * list,
    KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  const char * title() override;
  Escher::View * view() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int j) override;
  void setRecord(Ion::Storage::Record record);
private:
  Escher::StackViewController * stackController() const;
  Shared::Sequence * sequence() {
    assert(!m_record.isNull());
    return sequenceStore()->modelForRecord(m_record);
  }
  Shared::SequenceStore * sequenceStore();
  constexpr static int k_totalNumberOfCell = 3;
  Escher::ExpressionTableCellWithPointer m_explicitCell;
  Escher::ExpressionTableCellWithPointer m_singleRecurrenceCell;
  Escher::ExpressionTableCellWithPointer m_doubleRecurenceCell;
  Poincare::Layout m_layouts[k_totalNumberOfCell];
  Escher::SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  ListController * m_listController;
};

}

#endif
