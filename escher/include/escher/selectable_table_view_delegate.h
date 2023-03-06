#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DELEGATE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DELEGATE_H

#include <escher/context_provider.h>

namespace Escher {

class SelectableTableView;

class SelectableTableViewDelegate : public ContextProvider {
 public:
  /* withinTemporarySelection flag indicates when the selection change happens
   * in a temporary deselection: indeed, when reloading the data of the table,
   * we deselect the table before re-layouting the entire table and re-select
   * the previous selected cell. We might implement different course of action
   * when the selection change is 'real' or within temporary selection. */
  virtual void tableViewDidChangeSelection(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      bool withinTemporarySelection = false) {}
  virtual void tableViewDidChangeSelectionAndDidScroll(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      bool withinTemporarySelection = false) {}
  virtual bool canStoreContentOfCellAtLocation(SelectableTableView* t, int col,
                                               int row) const {
    return true;
  }
};

}  // namespace Escher
#endif
