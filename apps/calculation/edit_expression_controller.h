#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include "../shared/text_field_delegate.h"
#include "history_controller.h"
#include "calculation_store.h"
#include "text_field.h"

namespace Calculation {
class HistoryController;

class EditExpressionController : public ViewController, public Shared::TextFieldDelegate {
public:
  EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore);
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  const char * textBody();
  void setTextBody(const char * text);
  bool textFieldDidFinishEditing(::TextField * textField, const char * text) override;
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
    constexpr static int k_separatorThickness = 1;
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
