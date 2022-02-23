#ifndef ESCHER_STACK_VIEW_CONTROLLER_H
#define ESCHER_STACK_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/stack_view.h>
#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <ion/ring_buffer.h>
#include <stdint.h>

namespace Escher {

constexpr uint8_t k_MaxNumberOfStacks = 7;
static_assert(k_MaxNumberOfStacks < 8, "Bit mask representation relies on less than 8 stacks (uint8_t).");

class StackViewController : public ViewController {
public:
  enum class Style {
    WhiteUniform,
    GrayGradation
  };
  StackViewController(Responder * parentResponder, ViewController * rootViewController);

  /* Push creates a new StackView and adds it */
  void push(ViewController * vc, Style style = Style::WhiteUniform, int styleStep = -1);
  void pop();
  void popUntilDepth(int depth, bool shouldSetupTopViewController);

  int depth() const { return m_numberOfChildren; }
  View * view() override { return &m_view; }
  ViewController * topViewController();
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void setupHeadersBorderOverlaping(bool headersOverlapHeaders = true, bool headersOverlapContent = false, KDColor headersContentBorderColor = Palette::GrayBright) {
    m_view.setupHeadersBorderOverlaping(headersOverlapHeaders, headersOverlapContent, headersContentBorderColor);
  }
  static constexpr uint8_t k_maxNumberOfChildren = k_MaxNumberOfStacks;
private:
  class Frame {
  public:
    Frame(ViewController * viewController = nullptr, KDColor textColor = Palette::GrayDarkMiddle, KDColor backgroundColor = KDColorWhite, KDColor separatorColor = Palette::GrayBright) :
      m_viewController(viewController),
      m_textColor(textColor),
      m_backgroundColor(backgroundColor),
      m_separatorColor(separatorColor) {}
    ViewController * viewController() { return m_viewController; }
    KDColor textColor() { return m_textColor; }
    KDColor backgroundColor() { return m_backgroundColor; }
    KDColor separatorColor() { return m_separatorColor; }
  private:
    ViewController * m_viewController;
    KDColor m_textColor;
    KDColor m_backgroundColor;
    KDColor m_separatorColor;
  };
  class ControllerView : public View {
  public:
    ControllerView();
    void setHeadersDisplayMask(ViewController::TitlesDisplay mode) { m_headersDisplayMask = static_cast<uint8_t>(mode); }
    int8_t numberOfStacks() const { return m_numberOfStacks; }
    void setContentView(View * view);
    void setupHeadersBorderOverlaping(bool headersOverlapHeaders, bool headersOverlapContent, KDColor headersContentBorderColor);
    void pushStack(Frame frame);
    void popStack();
    bool shouldDisplayHeaderAtIndex(int i) const;
  protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    bool borderShouldOverlapContent() const;

    int numberOfDisplayedHeaders() const;
    // Returns the index in m_stackViews for a given display index
    int stackHeaderIndex(int displayIndex);

    StackView m_stackViews[k_MaxNumberOfStacks];
    SolidColorView m_borderView;
    View * m_contentView;
    int8_t m_numberOfStacks;
    /* Represents the stacks to display, _starting from the end_.
     * m_headersDisplayMask = 0b11111011   ->  shouldn't display m_stackViews[m_numberOfStacks - 1 - 2]. */
    uint8_t m_headersDisplayMask;
    bool m_headersOverlapHeaders;
    bool m_headersOverlapContent;
  };
  ControllerView m_view;
  void pushModel(Frame frame);
  void setupActiveView();
  void setupActiveViewController();
  bool shouldStoreHeaderOnStack(ViewController * vc);
  Frame m_childrenFrame[k_maxNumberOfChildren];
  uint8_t m_numberOfChildren;
  bool m_isVisible;
};

}
#endif
