#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include <poincare/expression_layout.h>
#include "expression_field.h"
#include "../shared/text_field_delegate.h"
#include "../shared/expression_layout_field_delegate.h"
#include "history_controller.h"
#include "calculation_store.h"

namespace Calculation {
class HistoryController;

/* TODO: implement a split view */
class EditExpressionController final : public DynamicViewController, public Shared::TextFieldDelegate, public Shared::ExpressionLayoutFieldDelegate {
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

  /* ExpressionLayoutFieldDelegate */
  bool expressionLayoutFieldDidReceiveEvent(::ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidFinishEditing(::ExpressionLayoutField * expressionLayoutField, Poincare::ExpressionLayout * layout, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidAbortEditing(::ExpressionLayoutField * expressionLayoutField) override;
  void expressionLayoutFieldDidChangeSize(::ExpressionLayoutField * expressionLayoutField) override;

private:
  class ContentView final : public View {
  public:
    ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate) :
      View(),
      m_mainView(subview),
      m_layout(new Poincare::HorizontalLayout()),
      m_expressionField(parentResponder, m_textBody, k_bufferLength, m_layout, textFieldDelegate, expressionLayoutFieldDelegate) {
      m_textBody[0] = 0;
    }
    ~ContentView() {
      delete m_layout;
    }
    ContentView(const ContentView& other) = delete;
    ContentView(ContentView&& other) = delete;
    ContentView& operator=(const ContentView& other) = delete;
    ContentView& operator=(ContentView&& other) = delete;
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
    Poincare::ExpressionLayout * m_layout;
    ExpressionField m_expressionField;
  };
  View * loadView() override;
  void unloadView(View * view) override {
    delete view;
  }
  void reloadView();
  bool inputViewDidReceiveEvent(Ion::Events::Event event);
  bool inputViewDidFinishEditing(const char * text, Poincare::ExpressionLayout * layout);
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
