#ifndef CALCULATION_ADDITIONAL_OUTPUTS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <poincare/expression.h>

namespace Calculation {

class EditExpressionController;

class ListController : public Escher::StackViewController, public Escher::MemoizedListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  ListController(EditExpressionController * editExpressionController, Escher::SelectableTableViewDelegate * delegate = nullptr);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // ListController
  virtual void setExpression(Poincare::Expression e) = 0;
  virtual void setExactAndApproximateExpression(Poincare::Expression exactExpression, Poincare::Expression approximateExpression) {
    // WARNING: Default only uses exactExpression
    setExpression(exactExpression);
  }
  virtual int textAtIndex(char * buffer, size_t bufferSize, int index) = 0;

protected:
  class InnerListController : public ViewController {
  public:
    InnerListController(ListController * dataSource, Escher::SelectableTableViewDelegate * delegate = nullptr);
    const char * title() override { return I18n::translate(I18n::Message::AdditionalResults); }
    Escher::View * view() override { return &m_selectableTableView; }
    void didBecomeFirstResponder() override;
    Escher::SelectableTableView * selectableTableView() { return &m_selectableTableView; }
  private:
    Escher::SelectableTableView m_selectableTableView;
  };
  InnerListController m_listController;
  EditExpressionController * m_editExpressionController;
};

}

#endif
