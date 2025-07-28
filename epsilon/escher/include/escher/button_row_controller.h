#ifndef BUTTON_ROW_CONTROLLER_H
#define BUTTON_ROW_CONTROLLER_H

#include <assert.h>
#include <escher/button_cell.h>
#include <escher/i18n.h>
#include <escher/invocation.h>
#include <escher/view.h>
#include <escher/view_controller.h>

namespace Escher {

class ButtonRowDelegate;

class ButtonRowController : public ViewController {
 public:
  enum class Position { Top, Bottom };
  enum class Style { PlainWhite, EmbossedGray };
  enum class Size { Small, Large };

  ButtonRowController(Responder* parentResponder,
                      ViewController* mainViewController,
                      ButtonRowDelegate* delegate,
                      Position position = Position::Top,
                      Style = Style::PlainWhite, Size size = Size::Small,
                      KDCoordinate verticalMargin = 0);

  View* view() override { return &m_contentView; }
  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  int selectedButton();
  bool setSelectedButton(int selectedButton);
  void setMessageOfButtonAtIndex(I18n::Message message, int index);
  void reloadButtons() { m_contentView.layoutSubviews(); }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  class ContentView : public View {
   public:
    ContentView(ViewController* mainViewController, ButtonRowDelegate* delegate,
                Position position, Style style, Size size,
                KDCoordinate verticalMargin);

    int numberOfSubviews() const override;
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void drawRect(KDContext* ctx, KDRect rect) const override;

    bool setSelectedButton(int selectedButton);
    int selectedButton() const { return m_selectedButton; }
    ViewController* mainViewController() const { return m_mainViewController; }
    int numberOfButtons() const;
    ButtonCell* buttonAtIndex(int index) const;
    void reload();

   private:
    void drawRowFrame(KDContext* ctx, KDCoordinate innerHeight,
                      KDColor backgroundColor, KDColor borderColor) const;
    KDCoordinate buttonRowHeight() const;

    ViewController* m_mainViewController;
    int m_selectedButton;
    ButtonRowDelegate* m_delegate;
    Position m_position;
    Style m_style;
    Size m_size;
    KDCoordinate m_verticalMargin;
  };

  void privateModalViewAltersFirstResponder(
      FirstResponderAlteration alteration) override;

  ContentView m_contentView;
};

class ButtonRowDelegate {
 public:
  ButtonRowDelegate(ButtonRowController* header, ButtonRowController* footer);

  virtual int numberOfButtons(ButtonRowController::Position position) const {
    return 0;
  }
  virtual ButtonCell* buttonAtIndex(
      int index, ButtonRowController::Position position) const {
    assert(false);
    return nullptr;
  }
  ButtonRowController* header() { return m_header; }
  ButtonRowController* footer() { return m_footer; }

 private:
  ButtonRowController* m_header;
  ButtonRowController* m_footer;
};

}  // namespace Escher

#endif
