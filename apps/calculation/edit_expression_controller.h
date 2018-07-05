#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include <poincare/layout_reference.h>
#include "expression_field.h"
#include "../shared/text_field_delegate.h"
#include "../shared/layout_field_delegate.h"
#include "history_controller.h"
#include "calculation_store.h"

namespace Calculation {
class HistoryController;

/* TODO: implement a split view */
class EditExpressionController : public DynamicViewController, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore);
  void didBecomeFirstResponder() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void insertTextBody(const char * text);

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(::TextField * textField) override;

  /* LayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(::LayoutField * layoutField, Poincare::LayoutRef layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(::LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(::LayoutField * layoutField) override;

private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
    void reload();
    TableView * mainView() { return m_mainView; }
    ExpressionField * expressionField() { return &m_expressionField; }
    /* View */
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
  private:
    static constexpr int k_bufferLength = TextField::maxBufferSize();
    TableView * m_mainView;
    char m_textBody[k_bufferLength];
    ExpressionField m_expressionField;
  };
  View * loadView() override;
  void unloadView(View * view) override;
  void reloadView();
  bool inputViewDidReceiveEvent(Ion::Events::Event event);
  bool inputViewDidFinishEditing(const char * text, Poincare::LayoutRef layoutR);
  bool inputViewDidAbortEditing(const char * text);
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  Shared::ExpressionFieldDelegateApp * expressionFieldDelegateApp() override;
  char m_cacheBuffer[Calculation::k_printedExpressionSize];
  HistoryController * m_historyController;
  CalculationStore * m_calculationStore;
  bool m_inputViewHeightIsMaximal;
};

}

#endif
