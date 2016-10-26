#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DELEGATE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DELEGATE_H

class SelectableTableView;

class SelectableTableViewDelegate {
public:
  virtual void tableViewDidChangeSelection(SelectableTableView * t) = 0;
};

#endif
