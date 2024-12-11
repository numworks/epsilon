#ifndef ESCHER_STACK_VIEW_CONTROLLER_H
#define ESCHER_STACK_VIEW_CONTROLLER_H

#include <escher/palette.h>
#include <escher/stack_view.h>
#include <escher/view_controller.h>
#include <stdint.h>

namespace Escher {

/* Use StackViewController::Default when possible. If the app needs more
 * view stack depth, use StackViewController::Custom<N> to have a custom
 * depth between 1 and 8. Always use pointers on StackViewController. */

template <unsigned Capacity>
class CustomSizeStackViewController;

class StackViewController : public ViewController {
 public:
  using Default = CustomSizeStackViewController<4>;
  template <int Depth>
  using Custom = CustomSizeStackViewController<Depth>;

  using Style = StackView::Style;

  /* Push creates a new StackView and adds it */
  void push(ViewController* vc);
  void pop();
  void popUntilDepth(int depth, bool shouldSetupTopViewController);

  int depth() const { return m_size; }
  StackView* view() override { return &m_view; }
  ViewController* topViewController();
  const ViewController* topViewController() const;
  const char* title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder* previousFirstResponder) override;
  void didBecomeFirstResponder() override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void setupActiveView();
  void setupHeadersBorderOverlaping(
      bool headersOverlapHeaders = true, bool headersOverlapContent = false,
      KDColor headersContentBorderColor = Palette::GrayBright) {
    m_view.setupHeadersBorderOverlaping(headersOverlapHeaders,
                                        headersOverlapContent,
                                        headersContentBorderColor);
  }

 protected:
  StackViewController(Responder* parentResponder, StackView::Style style,
                      bool extendVertically,
                      OMG::AbstractStack<StackHeaderView>* headerViewStack);

 private:
  StackView m_view;
  void pushModel(ViewController* vc);
  void setupActiveViewController();
  bool shouldStoreHeaderOnStack(ViewController* vc, int index);
  void updateStack(ViewController::TitlesDisplay titleDisplay);
  void dismissPotentialModal();
  virtual void didExitPage(ViewController* controller) const;
  virtual void willOpenPage(ViewController* controller) const;
  uint8_t m_size;
  bool m_isVisible;
  bool m_displayedAsModal;
  /* Represents the stacks to display, _starting from the end_.
   * m_headersDisplayMask = 0b11111011   ->  shouldn't display
   * m_stackViews[numberOfStacks - 1 - 2]. */
  StackView::Mask m_headersDisplayMask;

 private:
  virtual ViewController* stackSlot(int index) = 0;
  virtual const ViewController* stackSlot(int index) const = 0;

  virtual void setStackSlot(int index, ViewController* controller) = 0;
};

template <unsigned Capacity>
class CustomSizeStackViewController : public StackViewController {
 public:
  static_assert(Capacity <= StackView::k_maxDepth);

  using Stack = OMG::Stack<StackHeaderView, Capacity>;

  constexpr static int k_maxNumberOfChildren = Capacity;

  CustomSizeStackViewController(Responder* parentResponder,
                                ViewController* rootViewController,
                                StackView::Style style,
                                bool extendVertically = true)
      : StackViewController(parentResponder, style, extendVertically,
                            &m_headerViewStack) {
    m_stack[0] = rootViewController;
    rootViewController->setParentResponder(this);
  }

 private:
  ViewController* stackSlot(int index) override {
    assert(index >= 0 && index < static_cast<int>(Capacity));
    return m_stack[index];
  }

  const ViewController* stackSlot(int index) const override {
    assert(index >= 0 && index < static_cast<int>(Capacity));
    return m_stack[index];
  }

  void setStackSlot(int index, ViewController* controller) override {
    assert(index >= 0 && index < static_cast<int>(Capacity));
    m_stack[index] = controller;
  }

  ViewController* m_stack[Capacity];
  Stack m_headerViewStack;
};

}  // namespace Escher
#endif
