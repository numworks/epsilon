#ifndef SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H
#define SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H

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
  ExpressionModelListController(Escher::Responder* parentResponder,
                                I18n::Message text);
  virtual void editExpression(Ion::Events::Event event);
  virtual bool editSelectedRecordWithText(const char* text);
  virtual void getTextForSelectedRecord(char* text, size_t size) const;
  bool handleEventOnExpression(Ion::Events::Event event,
                               bool inTemplateMenu = false);
  bool handleEventOnExpressionInTemplateMenu(Ion::Events::Event event) {
    return handleEventOnExpression(event, true);
  }
  constexpr static KDCoordinate k_defaultVerticalMargin =
      Escher::Metric::CellMargins.left();
  /* We want all ListControllers to use the same margin */
  constexpr static KDCoordinate k_newModelMargin =
      k_defaultVerticalMargin + Escher::Metric::VerticalColorIndicatorThickness;
  constexpr static KDCoordinate k_defaultRowHeight =
      Escher::Metric::StoreRowHeight;

 protected:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_addNewModelCellType = 1;
  constexpr static int k_editableCellType = 2;
  constexpr static KDFont::Size k_font = KDFont::Size::Large;
  constexpr static KDCoordinate k_expressionMargin = 5;

  virtual Escher::EditableExpressionModelCell*
  editableExpressionModelCell() = 0;
  bool isAddEmptyRow(int row) const;

  // ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) const override {
    return type == k_expressionCellType ? maxNumberOfDisplayableRows() : 1;
  }
  int typeAtRow(int row) const override;
  // Row height
  virtual KDCoordinate expressionRowHeight(int row);
  virtual KDCoordinate editableRowHeight();
  KDCoordinate nonMemoizedRowHeight(int row) override;

  // Responder
  virtual void addNewModelAction();
  /* TODO: This should only update cells that changed instead of reloading the
   * whole memoization, which is time-consuming. */
  virtual void didChangeModelsList() {
    selectableListView()->resetSizeAndOffsetMemoization();
  }
  virtual bool removeModelRow(Ion::Storage::Record record);
  // Relative row is the index of the row starting from the model row
  int modelIndexForRow(int row, int* relativeRow) const;
  int modelIndexForRow(int row) const;
  Ion::Storage::Record recordAtRow(int row, int* relativeRow) const;
  Ion::Storage::Record recordAtRow(int row) const;
  Ion::Storage::Record selectedRecord(int* relativeRow) const;
  Ion::Storage::Record selectedRecord() const;

  // ViewController
  virtual Escher::SelectableListView* selectableListView() = 0;
  virtual ExpressionModelStore* modelStore() const = 0;
  Escher::EvenOddMessageTextCell m_addNewModelCell;

  // LayoutDelegate
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Ion::Events::Event event) override;
  void layoutFieldDidChangeSize(Escher::LayoutField* layoutField) override;
  void layoutFieldDidAbortEditing(Escher::LayoutField* layoutField) override;

  // EditableCell
  virtual Escher::LayoutField* layoutField() = 0;
  int16_t m_editedCellIndex;

 private:
  virtual int maxNumberOfDisplayableRows() const = 0;
  void finishEdition();
  bool addEmptyModel();
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
};

}  // namespace Shared

#endif
