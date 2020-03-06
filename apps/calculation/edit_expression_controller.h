#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include <poincare/layout.h>
#include "expression_field.h"
#include "../shared/text_field_delegate.h"
#include "../shared/layout_field_delegate.h"
#include "history_controller.h"
#include "calculation_store.h"

namespace Calculation {

/* TODO: implement a split view */
class EditExpressionController : public ViewController, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  EditExpressionController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, HistoryController * historyController, CalculationStore * calculationStore);
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void insertTextBody(const char * text);

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(::TextField * textField) override;

  /* LayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(::LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(::LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(::LayoutField * layoutField) override;

private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, TableView * subview, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
    void reload();
    TableView * mainView() { return m_mainView; }
    ExpressionField * expressionField() { return &m_expressionField; }
  private:
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    TableView * m_mainView;
    ExpressionField m_expressionField;
  };
  void reloadView();
  bool inputViewDidReceiveEvent(Ion::Events::Event event, bool shouldDuplicateLastCalculation);
  bool inputViewDidFinishEditing(const char * text, Poincare::Layout layoutR);
  bool inputViewDidAbortEditing(const char * text);
  static constexpr int k_cacheBufferSize = Constant::MaxSerializedExpressionSize;
  char m_cacheBuffer[k_cacheBufferSize];
  HistoryController * m_historyController;
  CalculationStore * m_calculationStore;
  ContentView m_contentView;
};

}

#endif
