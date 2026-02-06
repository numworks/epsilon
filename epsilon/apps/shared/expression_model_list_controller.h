#pragma once

#include <apps/i18n.h>
#include <escher/editable_expression_model_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/input_view_controller.h>
#include <escher/layout_field.h>
#include <escher/selectable_list_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>

#include "expression_model_store.h"
#include "math_field_delegate.h"

namespace Shared {

class ExpressionModelListController
    : public Escher::ViewController,
      public Escher::SelectableListViewDataSource,
      public Escher::SelectableListViewDelegate,
      public Escher::StandardMemoizedListViewDataSource,
      public MathLayoutFieldDelegate {
 public:
  constexpr static KDCoordinate k_defaultVerticalMargin =
      Escher::Metric::CellMargins.left();
  /* We want all ListControllers to use the same margin */
  constexpr static KDCoordinate k_newModelMargin =
      k_defaultVerticalMargin + Escher::Metric::VerticalColorIndicatorThickness;

  ExpressionModelListController(Escher::Responder* parentResponder,
                                I18n::Message text);

  bool handleEventOnExpressionInTemplateMenu(Ion::Events::Event event) {
    return handleEventOnExpression(event, true);
  }
  virtual void editExpression(Ion::Events::Event event);

  bool editSelectedRecordWithLayout(Poincare::Layout layout);
  virtual Poincare::Layout getLayoutForSelectedRecord() const;

 protected:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_addNewModelCellType = 1;
  constexpr static int k_editableCellType = 2;
  constexpr static KDFont::Size k_font = KDFont::Size::Large;
  constexpr static KDCoordinate k_expressionMargin = 5;

  // LayoutDelegate
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Ion::Events::Event event) override;
  void layoutFieldDidChangeSize(Escher::LayoutField* layoutField) override;
  void layoutFieldDidAbortEditing(Escher::LayoutField* layoutField) override;

  // ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) const override {
    return type == k_expressionCellType ? maxNumberOfDisplayableRows() : 1;
  }
  int typeAtRow(int row) const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;

  virtual Escher::EditableExpressionModelCell*
  editableExpressionModelCell() = 0;
  virtual void addNewModelAction() = 0;

  virtual bool setSelectedRecordContent(Poincare::Layout layout);
  // Row height
  virtual KDCoordinate expressionRowHeight(int row);
  virtual KDCoordinate editableRowHeight();
  /* TODO: This should only update cells that changed instead of reloading the
   * whole memoization, which is time-consuming. */
  virtual void didChangeModelsList() {
    selectableListView()->resetSizeAndOffsetMemoization();
  }
  virtual void removeModelRow(Ion::Storage::Record record);

  bool handleEventOnExpression(Ion::Events::Event event,
                               bool inTemplateMenu = false);
  // "Add element" row can be hidden if we reached the max number of models
  bool hasAddModelRow() const;
  bool isAddModelRow(int row) const;
  bool isEditing() const {
    assert(m_editedCellIndex >= 0 || !m_isAddingNewModel);
    return m_editedCellIndex >= 0;
  }
  // If the edited cell is a new model
  bool isAddingNewModel() const {
    assert(!m_isAddingNewModel || m_editedCellIndex >= 0);
    return m_isAddingNewModel;
  }
  bool isNewModelRow(int row) const;
  // Relative row is the index of the row starting from the model row
  int modelIndexForRow(int row, int* relativeRow) const;
  int modelIndexForRow(int row) const;
  // Record getters
  Ion::Storage::Record recordAtRow(int row, int* relativeRow) const;
  Ion::Storage::Record recordAtRow(int row) const;
  Ion::Storage::Record selectedRecord(int* relativeRow) const;
  Ion::Storage::Record selectedRecord() const;

  virtual Escher::LayoutField* layoutField() = 0;
  virtual Escher::SelectableListView* selectableListView() = 0;
  virtual ExpressionModelStore* modelStore() const = 0;

  Escher::EvenOddMessageTextCell m_addNewModelCell;

 private:
  constexpr static KDCoordinate k_defaultRowHeight =
      Escher::Metric::StoreRowHeight;

  // ListViewDataSource
  bool canStoreCellAtRow(int row) override {
    // Always disable storing cells since they are equations.
    return false;
  }

  virtual int maxNumberOfDisplayableRows() const = 0;

  virtual bool shouldCompleteEquation(Poincare::UserExpression expression,
                                      CodePoint symbol) {
    return false;
  }
  virtual bool completeEquation(Escher::LayoutField* equationField,
                                CodePoint symbol) {
    assert(false);
    return true;
  }
  virtual bool isValidExpressionModel(Poincare::UserExpression expression) {
    return true;
  }
  virtual int numberOfRowsForRecord(Ion::Storage::Record record) const {
    return 1;
  }

  void finishEdition();

  int16_t m_editedCellIndex;
  bool m_isAddingNewModel;
};

}  // namespace Shared
