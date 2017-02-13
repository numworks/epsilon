#ifndef ESCHER_STACK_VIEW_CONTROLLER_H
#define ESCHER_STACK_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/stack_view.h>
#include <escher/palette.h>

constexpr uint8_t kMaxNumberOfStacks = 4;

class StackViewController : public ViewController {
public:
  StackViewController(Responder * parentResponder, ViewController * rootViewController, bool displayFirstStackHeader = false,
    KDColor textColor = Palette::SubTab, KDColor backgroundColor = KDColorWhite, KDColor separatorColor = Palette::GreyBright);

  /* Push creates a new StackView and adds it */
  void push(ViewController * vc, KDColor textColor = Palette::SubTab, KDColor backgroundColor = KDColorWhite, KDColor separatorColor = Palette::GreyBright);
  void pop();


  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  class ControllerView : public View {
  public:
    ControllerView(bool displayFirstStackHeader);
    void setContentView(View * view);
    void pushStack(const char * name, KDColor textColor, KDColor backgroundColor, KDColor separatorColor);
    void popStack();
  protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  private:
    static constexpr KDCoordinate k_stackHeight = 20;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;

    StackView m_stackViews[kMaxNumberOfStacks];
    View * m_contentView;
    int8_t m_numberOfStacks;
    bool m_displayFirstStackHeader;
  };

  ControllerView m_view;

  void setupActiveViewController();
  static constexpr uint8_t k_maxNumberOfChildren = 4;
  ViewController * m_children[k_maxNumberOfChildren];
  uint8_t m_numberOfChildren;
  ViewController * m_rootViewController;
  KDColor m_textColor;
  KDColor m_backgroundColor;
  KDColor m_separatorColor;
};

#endif
