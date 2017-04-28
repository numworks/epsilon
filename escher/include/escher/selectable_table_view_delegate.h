#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DELEGATE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DELEGATE_H

class SelectableTableView;

class SelectableTableViewDelegate {
public:
  SelectableTableViewDelegate();
  int selectedRow();
  int selectedColumn();
  void selectRow(int j);
  void selectColumn(int i);
  void selectCellAtLocation(int i, int j);
  virtual void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY);
private:
  int m_selectedCellX;
  int m_selectedCellY;
};

#endif
