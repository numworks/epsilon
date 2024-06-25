#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include <apps/sequence/values/sequence_column_parameter_controller.h>
#include <apps/sequence/values/sum_column_parameter_controller.h>
#include <apps/shared/expression_function_title_cell.h>
#include <apps/shared/sequence_store.h>
#include <apps/shared/values_controller.h>

#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController,
                         Escher::RegularTableSize1DManager {
 public:
  ValuesController(Escher::Responder* parentResponder,
                   Escher::ButtonRowController* header);

  // ButtonRowDelegate
  Escher::ButtonCell* buttonAtIndex(
      int index,
      Escher::ButtonRowController::Position position) const override {
    return const_cast<Escher::SimpleButtonCell*>(&m_setIntervalButton);
  }

  // AlternateEmptyViewDelegate
  bool isEmpty() const override {
    return functionStore()->numberOfActiveFunctions() == 0;
  }
  I18n::Message emptyMessage() override {
    return functionStore()->numberOfDefinedModels() == 0
               ? I18n::Message::NoSequence
               : I18n::Message::NoActivatedSequence;
  }

  // Shared::ValuesController
  IntervalParameterController* intervalParameterController() override {
    return &m_intervalParameterController;
  }

 private:
  // TableViewDataSource
  Escher::TableSize1DManager* columnWidthManager() override {
    return &m_widthManager;
  }
  Escher::TableSize1DManager* rowHeightManager() override { return this; }

  // TableSize1DManager (height)
  constexpr static KDCoordinate k_sumLayoutHeight = 52;
  KDCoordinate computeSizeAtIndex(int i) override;
  KDCoordinate computeCumulatedSizeBeforeIndex(
      int i, KDCoordinate defaultSize) override;
  int computeIndexAfterCumulatedSize(KDCoordinate offset,
                                     KDCoordinate defaultSize) override;

  // ColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // EditableCellTableViewController
  bool checkDataAtLocation(double floatBody, int column,
                           int row) const override {
    return floatBody >= 0.0;
  }
  bool setDataAtLocation(double floatBody, int column, int row) override;

  // Shared::ValuesController
  Shared::SequenceStore* functionStore() const override {
    return static_cast<Shared::SequenceStore*>(
        Shared::ValuesController::functionStore());
  }
  Ion::Storage::Record recordAtColumn(int i) override {
    return recordAtColumn(i, nullptr);
  }
  Ion::Storage::Record recordAtColumn(int i, bool* isSumColumn);
  void updateNumberOfColumns() override;
  Poincare::Layout* memoizedLayoutAtIndex(int i) override;
  Poincare::Layout functionTitleLayout(int column) override;
  void setStartEndMessages(Shared::IntervalParameterController* controller,
                           int column) override {
    setDefaultStartEndMessages();
  }
  void createMemoizedLayout(int i, int j, int index) override;
  Shared::Interval* intervalAtColumn(int column) override;
  I18n::Message valuesParameterMessageAtColumn(int column) const override {
    return I18n::Message::N;
  }
  Shared::ExpressionFunctionTitleCell* functionTitleCells(int j) override;
  Escher::EvenOddExpressionCell* valueCells(int j) override;
  int abscissaCellsCount() const override {
    return k_maxNumberOfDisplayableRows;
  }
  Escher::AbstractEvenOddEditableTextCell* abscissaCells(int j) override;
  int abscissaTitleCellsCount() const override { return 1; }
  Escher::EvenOddMessageTextCell* abscissaTitleCells(int j) override;

  Escher::SelectableViewController* functionParameterController() override {
    return sequenceColumnParameterController();
  }
  Shared::ColumnParameters* functionParameters() override {
    return sequenceColumnParameterController();
  }
  Shared::ColumnParameterController* sequenceColumnParameterController();

  void setDefaultStartEndMessages();

  Shared::ExpressionFunctionTitleCell
      m_sequenceTitleCells[k_maxNumberOfDisplayableColumns];
  Escher::EvenOddExpressionCell m_valueCells[k_maxNumberOfDisplayableCells];
  Escher::EvenOddMessageTextCell m_abscissaTitleCell;
  Escher::EvenOddEditableTextCell<
      Poincare::PrintFloat::k_maxNumberOfSignificantDigits>
      m_abscissaCells[k_maxNumberOfDisplayableRows];

  IntervalParameterController m_intervalParameterController;
  SequenceColumnParameterController m_sequenceColumnParameterController;
  SumColumnParameterController m_sumColumnParameterController;
  Escher::SimpleButtonCell m_setIntervalButton;
  mutable Poincare::Layout m_memoizedLayouts[k_maxNumberOfDisplayableCells];
  mutable bool m_hasAtLeastOneSumColumn;

  Escher::RegularTableSize1DManager m_widthManager;
};

}  // namespace Sequence

#endif
