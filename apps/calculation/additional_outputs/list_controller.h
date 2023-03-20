#ifndef CALCULATION_ADDITIONAL_OUTPUTS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view.h>
#include <escher/selectable_list_view_data_source.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <poincare/expression.h>

namespace Calculation {

class EditExpressionController;

class ListController : public Escher::StackViewController,
                       public Escher::MemoizedListViewDataSource,
                       public Escher::SelectableListViewDataSource {
 public:
  ListController(EditExpressionController* editExpressionController,
                 Escher::SelectableListViewDelegate* delegate = nullptr);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // ListController
  virtual void setExpression(Poincare::Expression e) = 0;
  virtual void setExactAndApproximateExpression(
      Poincare::Expression exactExpression,
      Poincare::Expression approximateExpression) {
    // WARNING: Default only uses exactExpression
    setExpression(exactExpression);
  }
  virtual int textAtIndex(char* buffer, size_t bufferSize,
                          Escher::HighlightCell* cell, int index) = 0;

 protected:
  class InnerListController : public ViewController {
   public:
    InnerListController(ListController* dataSource,
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
};

}  // namespace Calculation

#endif
