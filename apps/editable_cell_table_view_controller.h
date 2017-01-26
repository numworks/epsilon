#ifndef APPS_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define APPS_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <escher.h>

class EditableCellTableViewController : public ViewController, public TableViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate {
public:
  EditableCellTableViewController(Responder * parentResponder, KDCoordinate topMargin,
    KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin);
  virtual View * view() override;

  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  int numberOfRows() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;

  void didBecomeFirstResponder() override;
protected:
  SelectableTableView m_selectableTableView;
private:
  static constexpr KDCoordinate k_cellHeight = 20;
  virtual bool cellAtLocationIsEditable(int columnIndex, int rowIndex) = 0;
  virtual void setDataAtLocation(float floatBody, int columnIndex, int rowIndex) = 0;
  virtual float dataAtLocation(int columnIndex, int rowIndex) = 0;
  virtual int numberOfElements() = 0;
  virtual int maxNumberOfElements() const = 0;
};

#endif
