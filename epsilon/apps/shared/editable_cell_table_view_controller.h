#ifndef SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H
#define SHARED_EDITABLE_CELL_TABLE_VIEW_CONTROLLER_H

#include <escher/even_odd_buffer_text_cell.h>
#include <escher/regular_table_view_data_source.h>
#include <escher/stack_view_controller.h>
#include <poincare/preferences.h>

#include "column_helper.h"
#include "column_parameter_controller.h"
#include "math_field_delegate.h"
#include "tab_table_controller.h"

namespace Shared {

class ColumnParameterController;

class EditableCellTableViewController
    : public TabTableController,
      public Escher::TableViewDataSource,
      public MathTextFieldDelegate,
      public ClearColumnHelper,
      public Escher::SelectableTableViewDelegate {
 public:
  EditableCellTableViewController(
      Responder* parentResponder,
      Escher::SelectableTableViewDelegate* delegate = nullptr);
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  int numberOfRows() const override;
  void fillCellForLocationWithDisplayMode(
      Escher::HighlightCell* cell, int column, int row,
      Poincare::Preferences::PrintFloatMode mode,
      uint8_t numberOfSignificantDigits);
  void viewWillAppear() override;

  bool handleEvent(Ion::Events::Event event) override;

  // SelectableTableViewDelegate
  int numberOfRowsAtColumn(const Escher::SelectableTableView* t,
                           int column) override {
    assert(t == &m_selectableTableView);
    return numberOfRowsAtColumn(column);
  }

 protected:
  constexpr static KDFont::Size k_cellFont = KDFont::Size::Small;
  constexpr static KDCoordinate k_cellHeight =
      Escher::Metric::SmallEditableCellHeight;
  constexpr static KDCoordinate k_cellWidth =
      Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
          Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision) *
          KDFont::GlyphWidth(k_cellFont) +
      2 * Escher::Metric::SmallCellMargin;

  constexpr static int k_maxNumberOfDisplayableRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          k_cellHeight, Escher::Metric::TabHeight);
  constexpr static int k_maxNumberOfDisplayableColumns =
      Ion::Display::Width / k_cellWidth + 2;
  constexpr static int k_maxNumberOfDisplayableCells =
      k_maxNumberOfDisplayableRows * k_maxNumberOfDisplayableColumns;

  // TabTableController
  Escher::SelectableTableView* selectableTableView() override {
    return &m_selectableTableView;
  }

  // TableViewDataSource
  KDCoordinate defaultRowHeight() override { return k_cellHeight; }
  KDCoordinate defaultColumnWidth() override { return k_cellWidth; }

  virtual void updateSizeMemoizationForRow(int row) {}

  // ClearColumnHelper
  Escher::SelectableTableView* table() override {
    return selectableTableView();
  }

  int numberOfRowsAtColumn(int column) const;

  /* Poor's man diamond inheritance */
  virtual Escher::SelectableViewController* columnParameterController() = 0;
  virtual Shared::ColumnParameters* columnParameters() = 0;
  virtual Escher::StackViewController* stackController() const = 0;
  virtual void setTitleCellText(Escher::HighlightCell* cell, int column) = 0;
  virtual void setTitleCellStyle(Escher::HighlightCell* cell, int column) = 0;
  virtual void reloadEditedCell(int column, int row) {
    selectableTableView()->reloadCellAtLocation(column, row);
  }
  virtual bool checkDataAtLocation(double floatBody, int column,
                                   int row) const {
    return true;
  }

  Escher::SelectableTableView m_selectableTableView;

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  virtual void didChangeCell(int column, int row) {}
  virtual bool cellAtLocationIsEditable(int column, int row) = 0;
  virtual bool setDataAtLocation(double floatBody, int column, int row) = 0;
  virtual double dataAtLocation(int column, int row) = 0;
  virtual int maxNumberOfElements() const = 0;
};

}  // namespace Shared

#endif
