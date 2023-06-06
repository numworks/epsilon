#ifndef CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view.h>
#include <escher/selectable_list_view_data_source.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <poincare/expression.h>

#include "additionnal_result_cell.h"

namespace Calculation {

class EditExpressionController;

class ExpressionsListController
    : public Escher::StackViewController,
      public Escher::StandardMemoizedListViewDataSource,
      public Escher::SelectableListViewDataSource {
 public:
  ExpressionsListController(
      EditExpressionController* editExpressionController,
      bool highlightWholeCells,
      Escher::SelectableListViewDelegate* delegate = nullptr);
  virtual ~ExpressionsListController() = default;
  void tidy();

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewDidDisappear() override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int numberOfRows() const override;

  virtual void setExpression(Poincare::Expression e);
  virtual void setExactAndApproximateExpression(
      Poincare::Expression exactExpression,
      Poincare::Expression approximateExpression) {
    // WARNING: Default only uses exactExpression
    setExpression(exactExpression);
  }
  virtual int textAtIndex(char* buffer, size_t bufferSize,
                          Escher::HighlightCell* cell, int index);

 protected:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_maxNumberOfRows = 5;
  Poincare::Layout getLayoutFromExpression(Poincare::Expression e,
                                           Poincare::Context* context,
                                           Poincare::Preferences* preferences);
  Poincare::Expression m_expression;
  // Memoization of layouts
  mutable Poincare::Layout m_layouts[k_maxNumberOfRows];
  mutable Poincare::Layout m_exactLayouts[k_maxNumberOfRows];
  mutable Poincare::Layout m_approximatedLayouts[k_maxNumberOfRows];
  AdditionnalResultCell m_cells[k_maxNumberOfRows];

  class InnerListController : public ViewController {
   public:
    InnerListController(ExpressionsListController* dataSource,
                        Escher::SelectableListViewDelegate* delegate = nullptr);
    const char* title() override {
      return I18n::translate(I18n::Message::AdditionalResults);
    }
    Escher::View* view() override { return &m_selectableListView; }
    void didBecomeFirstResponder() override;
    Escher::SelectableListView* selectableListView() {
      return &m_selectableListView;
    }

   private:
    Escher::SelectableListView m_selectableListView;
  };

  InnerListController m_listController;
  EditExpressionController* m_editExpressionController;

 private:
  virtual I18n::Message messageAtIndex(int index) = 0;
};

}  // namespace Calculation

#endif
