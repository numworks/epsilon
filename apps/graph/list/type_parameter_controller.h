#ifndef GRAPH_LIST_TYPE_PARAMATER_CONTROLLER_H
#define GRAPH_LIST_TYPE_PARAMATER_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/message_table_cell_with_expression.h>
#include <escher/stack_view_controller.h>
#include <ion/storage.h>

namespace Graph {

class TypeParameterController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  TypeParameterController(Responder * parentResponder);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char * title() override;
  View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override;
  TELEMETRY_ID("TypeParameter");

  // ListViewDataSource
  int numberOfRows() const override { return k_numberOfTypes; }
  KDCoordinate rowHeight(int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  MessageTableCellWithExpression * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfTypes; }
  int typeAtLocation(int i, int j) override { return 0; }

  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  constexpr static int k_numberOfTypes = 3;
  StackViewController * stackController() const;
  SelectableTableView m_selectableTableView;
  MessageTableCellWithExpression m_cells[k_numberOfTypes];
  Ion::Storage::Record m_record;
};

}

#endif
