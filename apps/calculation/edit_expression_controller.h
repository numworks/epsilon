#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include "../shared/text_field_delegate.h"
#include "history_controller.h"
#include "calculation_store.h"
#include "text_field.h"

namespace Calculation {
class HistoryController;

/* TODO: implement a split view */
class EditExpressionController : public DynamicViewController, public Shared::TextFieldDelegate {
public:
  EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore);
  void didBecomeFirstResponder() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  const char * textBody();
  void insertTextBody(const char * text);
bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(::TextField * textField, const char * text) override;
private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    TextField * textField();
    TableView * mainView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    static constexpr KDCoordinate k_textFieldHeight = 37;
    static constexpr KDCoordinate k_textMargin= 5;
    constexpr static int k_separatorThickness = 1;
    TableView * m_mainView;
    TextField m_textField;
    char m_textBody[TextField::maxBufferSize()];
  };
  View * loadView() override;
  void unloadView(View * view) override;
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  HistoryController * m_historyController;
  CalculationStore * m_calculationStore;
};

}

#endif
