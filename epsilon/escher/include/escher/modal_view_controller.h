#ifndef ESCHER_MODAL_VIEW_CONTROLLER_H
#define ESCHER_MODAL_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <kandinsky/margins.h>

namespace Escher {

class ModalViewController : public ViewController {
 public:
  ModalViewController(Responder* parentResponder, ViewController* child);
  View* view() override;

  bool handleEvent(Ion::Events::Event event) override;
  void displayModalViewController(ViewController* vc, float verticalAlignment,
                                  float horizontalAlignment,
                                  KDMargins margins = {},
                                  bool growingOnly = false);
  void reloadModal();
  void dismissModal();
  bool isDisplayingModal() const;
  void dismissPotentialModal() {
    if (isDisplayingModal()) {
      dismissModal();
    }
  }
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  ViewController* currentModalViewController() const {
    return m_currentModalViewController;
  };
  void setPreviousResponder(Responder* responder) {
    m_previousResponder = responder;
  }

 protected:
  void reloadView();
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  class ContentView : public View {
   public:
    ContentView();
    void setMainView(View* regularView);
    int numberOfSubviews() const override;
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void presentModalView(View* modalView, float verticalAlignment,
                          float horizontalAlignment, KDMargins margins,
                          bool growingOnly);
    void dismissModalView();
    bool isDisplayingModal() const;
    void reload();

   private:
    KDSize minimalSizeForOptimalDisplay() const override {
      return m_isDisplayingModal
                 ? m_currentModalView->minimalSizeForOptimalDisplay()
                 : m_regularView->minimalSizeForOptimalDisplay();
    }
    KDRect modalViewFrame() const;
    View* m_regularView;
    View* m_currentModalView;
    bool m_isDisplayingModal;
    float m_verticalAlignment;
    float m_horizontalAlignment;
    KDMargins m_margins;
    bool m_modalGrowingOnly;
  };

  ContentView m_contentView;
  Responder* m_previousResponder;
  ViewController* m_currentModalViewController;
  ViewController* m_regularViewController;
};

}  // namespace Escher
#endif
