#ifndef SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "text_field_delegate.h"
#include "tab_table_controller.h"
#include "regular_table_view_data_source.h"

namespace Shared {

class EditableCellTableViewController : public TabTableController , public RegularTableViewDataSource , public TextFieldDelegate {
public:
  EditableCellTableViewController(Responder * parentResponder, KDCoordinate topMargin,
    KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin);
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  int numberOfRows() override;
  void willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Poincare::Expression::FloatDisplayMode FloatDisplayMode);
  KDCoordinate rowHeight(int j) override;

  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
private:
  TextFieldDelegateApp * textFieldDelegateApp() override;
  static constexpr KDCoordinate k_cellHeight = 20;
  virtual bool cellAtLocationIsEditable(int columnIndex, int rowIndex) = 0;
  virtual bool setDataAtLocation(float floatBody, int columnIndex, int rowIndex) = 0;
  virtual float dataAtLocation(int columnIndex, int rowIndex) = 0;
  virtual int numberOfElements() = 0;
  virtual int maxNumberOfElements() const = 0;
};

}

#endif
