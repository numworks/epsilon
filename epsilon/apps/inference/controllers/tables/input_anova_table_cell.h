#pragma once

#include "categorical_table_cell.h"
#include "dynamic_cells_data_source.h"
#include "inference/models/anova_test.h"
#include "input_anova_data_source.h"

namespace Inference {

class InputANOVAController;

class InputANOVATableCell
    : public InputCategoricalTableCell,
      public InputANOVADataSource,
      public DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                          InferenceEvenOddEditableCell> {
 public:
  InputANOVATableCell(Escher::Responder* parentResponder, ANOVATest* test,
                      InputANOVAController* inputANOVAController,
                      Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }

  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  bool canStoreCellAtLocation(int column, int row) override {
    assert(column >= 0 && row >= 0);
    assert(column < k_maxNumberOfColumns && row < k_maxNumberOfRows);
    return column >= 0 && row > 0;
  }

  // DoubleDynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  int numberOfDynamicCellsType1() const override {
    return ANOVATableDimensions::k_numberOfInputHeaderReusableCells;
  }
  int numberOfDynamicCellsType2() const override {
    return ANOVATableDimensions::k_numberOfInputInnerReusableCells;
  }

  void initCellType2(InferenceEvenOddEditableCell* cell) override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  // DataSource
  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return cellType1(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return cellType2(i);
  }

 private:
  constexpr static int k_headerTranslationBufferSize = 20;

  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override { return column; }

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row);
  CategoricalController* categoricalController() override;

  InputANOVAController* m_inputANOVAController;
  I18n::Message m_headerPrefix;
};

}  // namespace Inference
