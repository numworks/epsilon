#ifndef SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <poincare/preferences.h>
#include <escher/regular_table_view_data_source.h>
#include <escher/stack_view_controller.h>
#include "column_helper.h"
#include "editable_cell_selectable_table_view.h"
#include "text_field_delegate.h"
#include "tab_table_controller.h"
#include "column_parameter_controller.h"

namespace Shared {

class ColumnParameterController;

class EditableCellTableViewController : public TabTableController , public Escher::TableViewDataSource, public TextFieldDelegate, public ClearColumnHelper {
public:
  EditableCellTableViewController(Responder * parentResponder, Escher::SelectableTableViewDelegate * delegate = nullptr);
  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  int numberOfRows() const override;
  void willDisplayCellAtLocationWithDisplayMode(Escher::HighlightCell * cell, int i, int j, Poincare::Preferences::PrintFloatMode mode);
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;

  bool handleEvent(Ion::Events::Event event) override;

  virtual int numberOfRowsAtColumn(int i) const = 0;

protected:
  constexpr static KDFont::Size k_cellFont = KDFont::Size::Small;
  constexpr static KDCoordinate k_cellHeight = Escher::Metric::SmallEditableCellHeight;
  constexpr static KDCoordinate k_cellWidth = Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits) * KDFont::GlyphWidth(k_cellFont) + 2*Escher::Metric::SmallCellMargin + Escher::Metric::TableSeparatorThickness;
  constexpr static KDCoordinate k_margin = Escher::Metric::TableSeparatorThickness;
  constexpr static KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;

  // TabTableController
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }

  // TableViewDataSource
  KDCoordinate defaultRowHeight() override { return k_cellHeight; }
  KDCoordinate defaultColumnWidth() override { return k_cellWidth; }

  virtual void updateSizeMemoizationForRow(int row, KDCoordinate rowPreviousHeight) {}

  // ClearColumnHelper
  Escher::SelectableTableView * table() override { return selectableTableView(); }

  /* Poor's man diamond inheritance */
  virtual Escher::SelectableViewController * columnParameterController() = 0;
  virtual Shared::ColumnParameters * columnParameters() = 0;
  virtual Escher::StackViewController * stackController() const = 0;
  virtual void setTitleCellText(Escher::HighlightCell * cell, int columnIndex) = 0;
  virtual void setTitleCellStyle(Escher::HighlightCell * cell, int columnIndex) = 0;
  virtual void reloadEditedCell(int column, int row) { selectableTableView()->reloadCellAtLocation(column, row); }
  virtual bool checkDataAtLocation(double floatBody, int columnIndex, int rowIndex) const { return true; }

  EditableCellSelectableTableView m_selectableTableView;

private:
  virtual void didChangeCell(int column, int row) {}
  virtual bool cellAtLocationIsEditable(int columnIndex, int rowIndex) = 0;
  virtual bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) = 0;
  virtual double dataAtLocation(int columnIndex, int rowIndex) = 0;
  virtual int maxNumberOfElements() const = 0;
};

}

#endif
