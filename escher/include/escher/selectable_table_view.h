#ifndef ESCHER_SELECTABLE_TABLE_VIEW_H
#define ESCHER_SELECTABLE_TABLE_VIEW_H

#include <escher/table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/table_view_data_source.h>
#include <escher/palette.h>
#include <escher/responder.h>

/* SelectableTableView is a Table View that handles selection. To implement it,
 * it needs a class which should be both data source and view controller. This
 * takes the selectable table view as instance variable and makes it first
 * responder. The selectable table view bubles up events when they do not
 * concern selection. */

class SelectableTableView : public TableView, public Responder {
public:
  SelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
      SelectableTableViewDataSource * selectionDataSource = nullptr, SelectableTableViewDelegate * delegate = nullptr);
  template <typename T> SelectableTableView(T * p) : SelectableTableView(p, p, p) {};
  void setDelegate(SelectableTableViewDelegate * delegate) { m_delegate = delegate; }
  int selectedRow();
  int selectedColumn();
  void selectRow(int j);
  void selectColumn(int i);
  void reloadData(bool setFirstResponder = true);
  virtual bool handleEvent(Ion::Events::Event event) override;
  virtual void didEnterResponderChain(Responder * previousFirstResponder) override;
  virtual void willExitResponderChain(Responder * nextFirstResponder) override;
  void deselectTable(bool withinTemporarySelection = false);
  bool selectCellAtLocation(int i, int j, bool setFirstResponder = true, bool withinTemporarySelection = false);
  HighlightCell * selectedCell();
protected:
  void unhighlightSelectedCell();
  SelectableTableViewDataSource * m_selectionDataSource;
  SelectableTableViewDelegate * m_delegate;
};

#endif
