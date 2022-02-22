#ifndef SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <poincare/preferences.h>
#include "column_parameter_controller.h"
#include "text_field_delegate.h"
#include "tab_table_controller.h"
#include "regular_table_view_data_source.h"
#include "pop_up_controller.h"

namespace Shared {

class EditableCellTableViewController : public TabTableController , public RegularTableViewDataSource, public TextFieldDelegate {
public:
  EditableCellTableViewController(Responder * parentResponder);
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;

  int numberOfRows() const override;
  void willDisplayCellAtLocationWithDisplayMode(Escher::HighlightCell * cell, int i, int j, Poincare::Preferences::PrintFloatMode mode);
  KDCoordinate rowHeight(int j) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  void tryToDeleteColumn(I18n::Message warningMessage1 = I18n::Message::ConfirmDeleteColumn1, I18n::Message warningMessage2 = I18n::Message::ConfirmDeleteColumn2);

  virtual bool handleEvent(Ion::Events::Event event) override;
  virtual void fillColumnName(int columnIndex, char * buffer) = 0;

protected:
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_margin = Escher::Metric::TableSeparatorThickness;
  static constexpr KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;

  virtual ColumnParameterController * columnParameterController() = 0;

private:
  virtual void didChangeCell(int column, int row) {}
  virtual bool cellAtLocationIsEditable(int columnIndex, int rowIndex) = 0;
  virtual bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) = 0;
  virtual double dataAtLocation(int columnIndex, int rowIndex) = 0;
  virtual int numberOfElementsInColumn(int columnIndex) const = 0;
  virtual int maxNumberOfElements() const = 0;
  virtual void deleteColumn() = 0;
  virtual bool isColumnDeletable(int columnIndex) { return true; }

  PopUpController m_confirmPopUpController;
};

}

#endif
