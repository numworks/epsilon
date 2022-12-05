#ifndef ESCHER_STACK_VIEW_CONTROLLER_H
#define ESCHER_STACK_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/stack_view.h>
#include <escher/palette.h>
#include <stdint.h>

namespace Escher {

constexpr uint8_t k_MaxNumberOfStacks = StackView::k_MaxNumberOfStacks;
static_assert(k_MaxNumberOfStacks < 8, "Bit mask representation relies on less than 8 stacks (uint8_t).");

class StackViewController : public ViewController {
public:
  typedef StackView::Style Style;
  StackViewController(Responder * parentResponder, ViewController * rootViewController, StackView::Style style, bool extendVertically = true);

  /* Push creates a new StackView and adds it */
  void push(ViewController * vc);
  void pop();
  void popUntilDepth(int depth, bool shouldSetupTopViewController);

  int depth() const { return m_numberOfChildren; }
  StackView * view() override { return &m_view; }
  ViewController * topViewController();
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  void didBecomeFirstResponder() override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void setupActiveView();
  void setupHeadersBorderOverlaping(bool headersOverlapHeaders = true, bool headersOverlapContent = false, KDColor headersContentBorderColor = Palette::GrayBright) {
    m_view.setupHeadersBorderOverlaping(headersOverlapHeaders, headersOverlapContent, headersContentBorderColor);
  }
  constexpr static uint8_t k_maxNumberOfChildren = k_MaxNumberOfStacks;
private:
  StackView m_view;
  void pushModel(ViewController * vc);
  void setupActiveViewController();
  bool shouldStoreHeaderOnStack(ViewController * vc, int index);
  void updateStack(ViewController::TitlesDisplay titleDisplay);
  void dismissPotentialModal();
  virtual void didExitPage(ViewController * controller) const;
  virtual void willOpenPage(ViewController * controller) const;
  ViewController * m_childrenController[k_maxNumberOfChildren];
  uint8_t m_numberOfChildren;
  bool m_isVisible;
  bool m_displayedAsModal;
  /* Represents the stacks to display, _starting from the end_.
   * m_headersDisplayMask = 0b11111011   ->  shouldn't display m_stackViews[numberOfStacks - 1 - 2]. */
  uint8_t m_headersDisplayMask;
};

}
#endif
