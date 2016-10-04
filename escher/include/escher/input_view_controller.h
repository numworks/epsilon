#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>

class InputViewController : public ViewController {
public:
  InputViewController(Responder * parentResponder, ViewController * child);
  View * view() override;
  const char * title() const override;

  bool handleEvent(Ion::Events::Event event) override;
  void edit(Responder * caller, const char * initialContent, void * context, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
private:
  class ContentView : public View {
  public:
    ContentView(View * subview);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void setVisibleInput(bool showInput);
    TextField * textField();
  private:
    static constexpr KDCoordinate k_textFieldHeight = 20;
    View * m_mainView;
    TextField m_textField;
    bool m_visibleInput;
    char m_textBody[255];
  };
  void showInput(bool show);
  void setTextBody(const char * text);
  void setPreviousResponder(Responder * previousResponder);
  ContentView m_contentView;
  Responder * m_previousResponder;
  Invocation m_successAction;
  Invocation m_failureAction;
};

#endif
