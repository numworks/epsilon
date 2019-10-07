#ifndef SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <escher.h>
#include <poincare/preferences.h>
#include "text_field_delegate.h"
#include "tab_table_controller.h"
#include "regular_table_view_data_source.h"

namespace Shared {

class EditableCellTableViewController : public TabTableController , public RegularTableViewDataSource, public TextFieldDelegate {
public:
  EditableCellTableViewController(Responder * parentResponder);
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;

  int numberOfRows() const override;
  void willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Poincare::Preferences::PrintFloatMode mode);
  KDCoordinate rowHeight(int j) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
protected:
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_margin = Metric::TableSeparatorThickness;
  static constexpr KDCoordinate k_scrollBarMargin = Metric::CommonRightMargin;
private:
  virtual void didChangeCell(int column, int row) {}
  virtual bool cellAtLocationIsEditable(int columnIndex, int rowIndex) = 0;
  virtual bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) = 0;
  virtual double dataAtLocation(int columnIndex, int rowIndex) = 0;
  virtual int numberOfElementsInColumn(int columnIndex) const = 0;
  virtual int maxNumberOfElements() const = 0;
};

}

#endif
