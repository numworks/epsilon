#ifndef CALCULATION_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_EXPRESSIONS_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view.h>
#include <escher/selectable_list_view_data_source.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <poincare/expression.h>

#include "additional_result_cell.h"

namespace Calculation {

class EditExpressionController;

class ExpressionsListController
    : public Escher::StackViewController::Default,
      public Escher::StandardMemoizedListViewDataSource,
      public Escher::SelectableListViewDataSource {
 public:
  ExpressionsListController(
      EditExpressionController* editExpressionController,
      bool highlightWholeCells,
      Escher::SelectableListViewDelegate* delegate = nullptr);
  virtual ~ExpressionsListController() = default;
  void tidy();

  void setCalculationPreferences(
      Poincare::Preferences::CalculationPreferences calculationPreferences) {
    m_calculationPreferences = calculationPreferences;
  }

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewDidDisappear() override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int numberOfRows() const override;

  virtual void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) = 0;
  virtual Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                         int index);

 protected:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_maxNumberOfRows = 5;
  Poincare::Layout getExactLayoutFromExpression(
      const Poincare::UserExpression e,
      const Poincare::Internal::ProjectionContext* ctx,
      Poincare::Layout* approximatedLayout = nullptr);
  Poincare::Preferences::AngleUnit angleUnit() const {
    return m_calculationPreferences.angleUnit;
  }
  Poincare::Preferences::PrintFloatMode displayMode() const {
    return m_calculationPreferences.displayMode;
  }
  Poincare::Preferences::ComplexFormat complexFormat() const {
    return m_calculationPreferences.complexFormat;
  }
  uint8_t numberOfSignificantDigits() const {
    return m_calculationPreferences.numberOfSignificantDigits;
  }

  // Memoization of layouts
  mutable Poincare::Layout m_layouts[k_maxNumberOfRows];
  mutable Poincare::Layout m_exactLayouts[k_maxNumberOfRows];
  mutable Poincare::Layout m_approximatedLayouts[k_maxNumberOfRows];
  AdditionalResultCell m_cells[k_maxNumberOfRows];

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
  Poincare::Preferences::CalculationPreferences m_calculationPreferences;

 private:
  virtual I18n::Message messageAtIndex(int index) = 0;
};

}  // namespace Calculation

#endif
