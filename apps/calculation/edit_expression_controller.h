#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include "history_controller.h"
#include "calculation_store.h"
#include "text_field.h"
#include "../shared/text_field_delegate.h"

namespace Calculation {
class HistoryController;

class EditExpressionController : public ViewController, public Shared::TextFieldDelegate {
public:
  EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore);
  View * view() override;
  const char * title() const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  const char * textBody();
  void setTextBody(const char * text);
  bool textFieldDidFinishEditing(::TextField * textField, const char * text) override;
private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    TextField * textField();
    TableView * mainView();
  private:
    static constexpr KDCoordinate k_textFieldHeight = 20;
    TableView * m_mainView;
    TextField m_textField;
    char m_textBody[255];
  };
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  ContentView m_contentView;
  HistoryController * m_historyController;
  CalculationStore * m_calculationStore;
};

}

#endif
