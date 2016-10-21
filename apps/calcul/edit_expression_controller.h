#ifndef CALCUL_EDIT_EXPRESSION_CONTROLLER_H
#define CALCUL_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include "history_controller.h"
#include "calcul_store.h"

namespace Calcul {
class HistoryController;

class EditExpressionController : public ViewController {
public:
  EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculStore * calculStore);
  View * view() override;
  const char * title() const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void edit(const char * initialContent);
  const char * textBody();
private:
  class ContentView : public View {
  public:
    ContentView(View * subview);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    TextField * textField();
  private:
    static constexpr KDCoordinate k_textFieldHeight = 20;
    View * m_mainView;
    TextField m_textField;
    char m_textBody[255];
  };
  void setTextBody(const char * text);
  ContentView m_contentView;
  HistoryController * m_historyController;
  CalculStore * m_calculStore;
};

}

#endif
