#ifndef BUTTON_ROW_CONTROLLER_H
#define BUTTON_ROW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/view.h>
#include <escher/invocation.h>
#include <escher/i18n.h>
#include <escher/button.h>
#include <assert.h>

class ButtonRowDelegate;

class ButtonRowController : public ViewController {
public:
  enum class Position {
    Top,
    Bottom
  };
  enum class Style {
    PlainWhite,
    EmbossedGrey
  };
  enum class Size {
    Small,
    Large
  };
  ButtonRowController(Responder * parentResponder, ViewController * mainViewController, ButtonRowDelegate * delegate, Position position = Position::Top, Style = Style::PlainWhite, Size size = Size::Small);
  View * view() override { return &m_contentView; }
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int selectedButton();
  bool setSelectedButton(int selectedButton);
  void setMessageOfButtonAtIndex(I18n::Message message, int index);
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  ViewController::DisplayParameter displayParameter() override { return DisplayParameter::DoNotShowOwnTitle; }
private:
  class ContentView : public View {
  public:
    ContentView(ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style, Size size);
    int numberOfButtons() const;
    Button * buttonAtIndex(int index) const;
    void reload();
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool setSelectedButton(int selectedButton);
    int selectedButton() const { return m_selectedButton; }
    ViewController * mainViewController() const { return m_mainViewController; }
    ButtonRowDelegate * buttonRowDelegate() const { return m_delegate; }
  private:
    constexpr static KDCoordinate k_plainStyleHeight = 20;
    constexpr static KDCoordinate k_embossedStyleHeightSmall = 36;
    constexpr static KDCoordinate k_embossedStyleHeightLarge = 52;
    constexpr static KDCoordinate k_embossedStyleHeightMarginSmall = 6;
    constexpr static KDCoordinate k_embossedStyleHeightMarginLarge = 8;
    ViewController * m_mainViewController;
    int m_selectedButton;
    ButtonRowDelegate * m_delegate;
    Position m_position;
    Style m_style;
    Size m_size;
  };
  ContentView m_contentView;
};

class ButtonRowDelegate {
public:
  ButtonRowDelegate(ButtonRowController * header, ButtonRowController * footer);
  virtual int numberOfButtons(ButtonRowController::Position position) const { return 0; }
  virtual Button * buttonAtIndex(int index, ButtonRowController::Position position) const {
    assert(false);
    return nullptr;
  }
  ButtonRowController * header() { return m_header; }
  ButtonRowController * footer() { return m_footer; }
private:
  ButtonRowController * m_header;
  ButtonRowController * m_footer;
};

#endif
