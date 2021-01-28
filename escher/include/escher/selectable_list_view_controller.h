#ifndef ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H
#define ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_delegate.h>

namespace Escher {
class SelectableListViewController : public ViewController, public MemoizedListViewDataSource, public SelectableTableViewDataSource {
public:
  SelectableListViewController(Responder * parentResponder, SelectableTableViewDelegate * tableDelegate = nullptr) :
    ViewController(parentResponder),
    MemoizedListViewDataSource(),
    m_selectableTableView(this, this, this, tableDelegate)
  {}
  /* ViewController */
  Escher::View * view() override { return &m_selectableTableView; }
  /* MemoizedListViewDataSource */
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
protected:
  SelectableTableView m_selectableTableView;
};

}

#endif
