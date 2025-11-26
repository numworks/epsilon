#pragma once

#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <shared/column_helper.h>
#include <shared/math_field_delegate.h>
#include <shared/parameter_text_field_delegate.h>

#include "categorical_table_view_data_source.h"
#include "dynamic_cells_data_source.h"
#include "inference/models/inference_model.h"
#include "inference/models/input_table.h"

namespace Inference {

class CategoricalController;

class CategoricalTableCell : public Escher::HighlightCell,
                             public Escher::Responder,
                             public Escher::SelectableTableViewDataSource,
                             public Escher::SelectableTableViewDelegate {
 public:
  CategoricalTableCell(Escher::Responder* parentResponder,
                       Escher::TableViewDataSource* dataSource,
                       Escher::ScrollViewDelegate* scrollViewDelegate);

  // Responder
  bool handleEvent(Ion::Events::Event e) override;

  // HighlightCell
  void reloadCell() override {}
  Responder* responder() override { return this; }

  Escher::SelectableTableView* selectableTableView() {
    return &m_selectableTableView;
  }
  virtual CategoricalTableViewDataSource* tableViewDataSource() = 0;

  KDSize minimalSizeForOptimalDisplay() const override {
    return m_selectableTableView.minimalSizeForOptimalDisplay();
  }
  KDSize contentSizeWithMargins() const {
    return m_selectableTableView.contentSizeWithMargins();
  }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelectionAndDidScroll(
      Escher::SelectableTableView* t, int previousSelectedCol,
      int previousSelectedRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) override;

  void layoutSubviews(bool force = false) override;

 protected:
  constexpr static int k_bottomMargin = 5;
  virtual CategoricalController* categoricalController() = 0;

  Escher::SelectableTableView m_selectableTableView;

  void handleResponderChainEvent(ResponderChainEvent event) override;

  virtual int relativeColumn(int column) const = 0;
  virtual int relativeRow(int row) const = 0;

 private:
  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int i) override {
    return &m_selectableTableView;
  }
};

class InputCategoricalTableCell : public CategoricalTableCell,
                                  public Shared::MathTextFieldDelegate,
                                  public Shared::ClearColumnHelper {
 public:
  InputCategoricalTableCell(Escher::Responder* parentResponder,
                            Escher::TableViewDataSource* dataSource,
                            InferenceModel* inference,
                            Escher::ScrollViewDelegate* scrollViewDelegate);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  // Responder
  bool handleEvent(Ion::Events::Event e) override;

  bool recomputeDimensions();
  virtual bool recomputeDimensionsAndReload(bool forceReloadTable = false,
                                            bool forceReloadPage = false,
                                            bool forceReloadCell = false);
  InputTable* tableModel() { return m_inference->table(); }

 protected:
  // ClearColumnHelper
  Escher::SelectableTableView* table() override {
    return selectableTableView();
  }
  int numberOfElementsInColumn(int column) const override;
  void clearSelectedColumn() override;
  bool isColumnClearable(int column) override {
    return relativeColumn(column) >= 0;
  }

  bool deleteSelectedValue();
  const InputTable* constTableModel() const {
    return const_cast<InputCategoricalTableCell*>(this)->tableModel();
  }

  InferenceModel* m_inference;
  int m_numberOfRows;
  int m_numberOfColumns;
};

namespace DoubleColumnTableDimensions {
constexpr static int k_maxNumberOfColumns =
    InputTable::k_maxNumberOfStoreColumns;
constexpr static int k_numberOfReusableCells =
    k_maxNumberOfColumns *
    CategoricalTableViewDataSource::k_maxNumberOfReusableRows;

};  // namespace DoubleColumnTableDimensions

class DoubleColumnTableCell
    : public InputCategoricalTableCell,
      public CategoricalTableViewDataSource,
      public DynamicCellsDataSource<InferenceEvenOddEditableCell> {
 public:
  constexpr static int k_maxNumberOfColumns =
      DoubleColumnTableDimensions::k_maxNumberOfColumns;

  DoubleColumnTableCell(Escher::Responder* parentResponder,
                        InferenceModel* inference,
                        Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // DataSource
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;
  int typeAtLocation(int column, int row) const override {
    return row < numberOfHeaderRows() ? k_typeOfHeaderCells
                                      : k_typeOfInnerCells;
  }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  void initCell(InferenceEvenOddEditableCell* cell) override;

  int numberOfDynamicCells() const override {
    return DoubleColumnTableDimensions::k_numberOfReusableCells;
  }

 protected:
  int numberOfHeaderRows() const override { return 1; }
  int numberOfHeaderColumns() const override { return 0; }

  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }

 private:
  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return k_columnWidth;
  }
  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override {
    return column - numberOfHeaderColumns();
  }
  int relativeRow(int row) const override { return row - numberOfHeaderRows(); }
  virtual Escher::HighlightCell* reusableHeaderCell(int index) = 0;
};

}  // namespace Inference
