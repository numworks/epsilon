#ifndef BUTTON_ROW_CONTROLLER_H
#define BUTTON_ROW_CONTROLLER_H

#include <escher/abstract_button_cell.h>
#include <escher/i18n.h>
#include <escher/invocation.h>
#include <escher/view.h>
#include <escher/view_controller.h>
#include <assert.h>

namespace Escher {

class ButtonRowDelegate;

class ButtonRowController : public ViewController {
public:
  enum class Position { Top, Bottom };
  enum class Style { PlainWhite, EmbossedGray };
  enum class Size { Small, Large };

  ButtonRowController(Responder * parentResponder, ViewController * mainViewController, ButtonRowDelegate * delegate, Position position = Position::Top, Style = Style::PlainWhite, Size size = Size::Small);

  View * view() override { return &m_contentView; }
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  bool handleEvent(Ion::Events::Event event) override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  int selectedButton();
  bool setSelectedButton(int selectedButton);
  void setMessageOfButtonAtIndex(I18n::Message message, int index);
  void reloadButtons() { m_contentView.layoutSubviews(); }

private:
  class ContentView : public View {
  public:
    ContentView(ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style, Size size);

    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void drawRect(KDContext * ctx, KDRect rect) const override;

    bool setSelectedButton(int selectedButton);
    int selectedButton() const { return m_selectedButton; }
    ViewController * mainViewController() const { return m_mainViewController; }
    int numberOfButtons() const;
    AbstractButtonCell * buttonAtIndex(int index) const;
    void reload();

  private:
    constexpr static KDCoordinate k_plainStyleHeight = Metric::ButtonRowPlainStyleHeight;
    constexpr static KDCoordinate k_embossedStyleHeightSmall = Metric::ButtonRowEmbossedStyleHeightSmall;
    constexpr static KDCoordinate k_embossedStyleHeightLarge = Metric::ButtonRowEmbossedStyleHeightLarge;
    constexpr static KDCoordinate k_embossedStyleHeightMarginSmall = Metric::ButtonRowEmbossedStyleHeightMarginSmall;
    constexpr static KDCoordinate k_embossedStyleHeightMarginLarge = Metric::ButtonRowEmbossedStyleHeightMarginLarge;

    ViewController * m_mainViewController;
    int m_selectedButton;
    ButtonRowDelegate * m_delegate;
    Position m_position;
    Style m_style;
    Size m_size;
  };

  void privateModalViewAltersFirstResponder(FirstResponderAlteration alteration) override;

  ContentView m_contentView;
};

class ButtonRowDelegate {
public:
  ButtonRowDelegate(ButtonRowController * header, ButtonRowController * footer);

  virtual int numberOfButtons(ButtonRowController::Position position) const { return 0; }
  virtual AbstractButtonCell * buttonAtIndex(int index, ButtonRowController::Position position) const { assert(false); return nullptr; }
  ButtonRowController * header() { return m_header; }
  ButtonRowController * footer() { return m_footer; }

private:
  ButtonRowController * m_header;
  ButtonRowController * m_footer;
};

}

#endif
