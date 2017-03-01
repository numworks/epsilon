#ifndef BUTTON_ROW_CONTROLLER_H
#define BUTTON_ROW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/view.h>
#include <escher/invocation.h>
#include <escher/button.h>
#include <escher/app.h>

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
  ButtonRowController(Responder * parentResponder, ViewController * mainViewController, ButtonRowDelegate * delegate, Position position = Position::Top, Style = Style::PlainWhite);
  View * view() override;
  const char * title() const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool setSelectedButton(int selectedButton);
  void viewWillAppear() override;
  void viewWillDisappear() override;
private:
  class ContentView : public View {
  public:
    ContentView(ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style);
    int numberOfButtons() const;
    Button * buttonAtIndex(int index) const;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool setSelectedButton(int selectedButton, App * app);
    int selectedButton();
    ViewController * mainViewController() const;
    ButtonRowDelegate * buttonRowDelegate() const;
  private:
    constexpr static KDCoordinate k_plainStyleHeight = 20;
    constexpr static KDCoordinate k_embossedStyleHeight = 46;
    constexpr static KDCoordinate k_embossedStyleHeightMargin = 5;
    constexpr static KDColor k_separatorHeaderColor = KDColor::RGB24(0xDEE0E2);
    constexpr static KDColor k_selectedBackgroundColor = KDColor::RGB24(0x426DA7);
    ViewController * m_mainViewController;
    int m_selectedButton;
    ButtonRowDelegate * m_delegate;
    Position m_position;
    Style m_style;
  };
  ContentView m_contentView;
};

class ButtonRowDelegate {
public:
  ButtonRowDelegate(ButtonRowController * header, ButtonRowController * footer);
  virtual int numberOfButtons(ButtonRowController::Position position) const;
  virtual Button * buttonAtIndex(int index, ButtonRowController::Position position) const;
  ButtonRowController * header();
  ButtonRowController * footer();
private:
  ButtonRowController * m_header;
  ButtonRowController * m_footer;
};

#endif
