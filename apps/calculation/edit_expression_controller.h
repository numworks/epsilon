#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <apps/shared/math_field_delegate.h>
#include <poincare/layout.h>

#include "calculation_selectable_list_view.h"
#include "history_controller.h"
#include "layout_field.h"

namespace Calculation {

/* TODO: implement a split view */
class EditExpressionController : public Escher::ViewController,
                                 public Shared::MathLayoutFieldDelegate {
 public:
  /* k_layoutBufferMaxSize dictates the size under which the expression being
   * edited can be remembered when the user leaves Calculation. */
  constexpr static int k_layoutBufferMaxSize = 1024;
  /* k_cacheBufferSize is the size of the array to which m_cacheBuffer points.
   * It is used both as a way to buffer expression when pushing them the
   * CalculationStore, and as a storage for the current input when leaving the
   * application. */
  constexpr static int k_cacheBufferSize =
      std::max(k_layoutBufferMaxSize, Constant::MaxSerializedExpressionSize);

  EditExpressionController(Escher::Responder* parentResponder,
                           HistoryController* historyController,
                           CalculationStore* calculationStore);

  /* ViewController */
  Escher::View* view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  /* MathLayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  void layoutFieldDidHandleEvent(Escher::LayoutField* layoutField) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Ion::Events::Event event) override;
  void layoutFieldDidChangeSize(Escher::LayoutField* layoutField) override;
  bool isAcceptableExpression(const Poincare::Expression expression,
                              Poincare::Context* context) override;
  bool shouldInsertTextForStoEvent(
      Escher::LayoutField* layoutField) const override {
    return true;
  }
  bool shouldInsertTextForAnsEvent(
      Escher::LayoutField* layoutField) const override {
    return true;
  }

  void insertLayout(Poincare::Layout layout);
  void restoreInput();

 private:
  class ContentView : public Escher::View {
   public:
    ContentView(Escher::Responder* parentResponder,
                CalculationSelectableListView* subview,
                Escher::LayoutFieldDelegate* layoutFieldDelegate);
    void reload();
    CalculationSelectableListView* mainView() { return m_mainView; }
    LayoutField* layoutField() { return m_expressionInputBar.layoutField(); }

   private:
    int numberOfSubviews() const override { return 2; }
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    CalculationSelectableListView* m_mainView;
    ExpressionInputBar m_expressionInputBar;
  };

  void reloadView();
  void memoizeInput();

  HistoryController* m_historyController;
  CalculationStore* m_calculationStore;
  ContentView m_contentView;
};

}  // namespace Calculation

#endif
