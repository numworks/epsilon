#ifndef GRAPH_LIST_TYPE_PARAMATER_CONTROLLER_H
#define GRAPH_LIST_TYPE_PARAMATER_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/message_table_cell_with_expression.h>
#include <escher/stack_view_controller.h>
#include <ion/storage.h>

namespace Graph {

class TypeParameterController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  TypeParameterController(Escher::Responder * parentResponder);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char * title() override;
  Escher::View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override;
  TELEMETRY_ID("TypeParameter");

  // ListViewDataSource
  KDCoordinate cellWidth() override {
    assert(m_selectableTableView.columnWidth(0) > 0);
    return m_selectableTableView.columnWidth(0);
    // assert(m_selectableTableView.columnWidth(0) == 280);
    // return m_selectableTableView.columnWidth(0);
  }
  int numberOfRows() const override { return k_numberOfTypes; }
  KDCoordinate rowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  Escher::MessageTableCellWithExpression * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfTypes; }
  int typeAtLocation(int i, int j) override { return 0; }

  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  constexpr static int k_numberOfTypes = 3;
  Escher::StackViewController * stackController() const;
  Escher::SelectableTableView m_selectableTableView;
  Escher::MessageTableCellWithExpression m_cells[k_numberOfTypes];
  Ion::Storage::Record m_record;
};

}

#endif
