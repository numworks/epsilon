#ifndef ESCHER_ALTERNATE_EMPTY_VIEW_H
#define ESCHER_ALTERNATE_EMPTY_VIEW_H

#include <escher/alternate_empty_view_delegate.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>
#include <escher/view_controller.h>

class AlternateEmptyViewController : public ViewController {
public:
  AlternateEmptyViewController(Responder * parentResponder, ViewController * mainViewController, AlternateEmptyViewDelegate * delegate);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  class ContentView : public View {
  public:
    ContentView(ViewController * mainViewController, AlternateEmptyViewDelegate * delegate);
    ViewController * mainViewController() const;
    AlternateEmptyViewDelegate * alternateEmptyViewDelegate() const;
    void layoutSubviews() override;
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    MessageTextView m_message;
    ViewController * m_mainViewController;
    AlternateEmptyViewDelegate * m_delegate;
  };
  ContentView m_contentView;
};

#endif
