#ifndef ESCHER_MODAL_VIEW_CONTROLLER_H
#define ESCHER_MODAL_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/stack_view.h>

class ModalViewController : public ViewController {
public:
  ModalViewController(Responder * parentResponder, ViewController * child);
  View * view() override;
  const char * title() const override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment);
  void dismissModalViewController();
  bool isDisplayingModal();
private:
  class ContentView : public View {
  public:
    ContentView();
    void setMainView(View * regularView);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void presentModalView(View * modalView, float verticalAlignment, float horizontalAlignment);
    void dismissModalView();
    bool isDisplayingModal() const;
  private:
    KDRect frame();
    View * m_regularView;
    View * m_currentModalView;
    bool m_isDisplayingModal;
    float m_verticalAlignment;
    float m_horizontalAlignment;
  };
  ContentView m_contentView;
  Responder * m_previousResponder;
  ViewController * m_currentModalViewController;
  ViewController * m_regularViewController;
};

#endif
