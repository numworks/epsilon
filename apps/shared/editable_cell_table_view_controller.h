#ifndef SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <poincare/preferences.h>
#include <escher/stack_view_controller.h>
#include "column_helper.h"
#include "text_field_delegate.h"
#include "tab_table_controller.h"
#include "regular_table_view_data_source.h"

namespace Shared {

class ColumnParameterController;

class EditableCellTableViewController : public TabTableController , public RegularTableViewDataSource, public TextFieldDelegate, public ClearColumnHelper {
public:
  EditableCellTableViewController(Responder * parentResponder);
  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  int numberOfRows() const override;
  void willDisplayCellAtLocationWithDisplayMode(Escher::HighlightCell * cell, int i, int j, Poincare::Preferences::PrintFloatMode mode);
  KDCoordinate rowHeight(int j) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;

  bool handleEvent(Ion::Events::Event event) override;

protected:
  constexpr static KDCoordinate k_cellHeight = 20;
  constexpr static KDCoordinate k_margin = Escher::Metric::TableSeparatorThickness;
  constexpr static KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;

  // ClearColumnHelper
  Escher::SelectableTableView * table() override { return selectableTableView(); }

  virtual ColumnParameterController * columnParameterController() = 0;
  virtual Escher::StackViewController * stackController() const = 0;
  virtual void setTitleCellText(Escher::HighlightCell * cell, int columnIndex) = 0;
  virtual void setTitleCellStyle(Escher::HighlightCell * cell, int columnIndex) = 0;
  virtual void reloadEditedCell(int column, int row) { selectableTableView()->reloadCellAtLocation(column, row); }
  virtual bool checkDataAtLocation(double floatBody, int columnIndex, int rowIndex) const { return true; }

private:
  virtual void didChangeCell(int column, int row) {}
  virtual bool cellAtLocationIsEditable(int columnIndex, int rowIndex) = 0;
  virtual bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) = 0;
  virtual double dataAtLocation(int columnIndex, int rowIndex) = 0;
  virtual int maxNumberOfElements() const = 0;
};

}

#endif
