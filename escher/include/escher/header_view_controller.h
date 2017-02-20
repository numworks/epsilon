#ifndef HEADER_VIEW_CONTROLLER_H
#define HEADER_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/view.h>
#include <escher/invocation.h>
#include <escher/button.h>
#include <escher/app.h>
#include <escher/header_view_delegate.h>

class HeaderViewController : public ViewController {
public:
  HeaderViewController(Responder * parentResponder, ViewController * mainViewController, HeaderViewDelegate * delegate);
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
    ContentView(ViewController * mainViewController, HeaderViewDelegate * delegate);
    int numberOfButtons() const;
    Button * buttonAtIndex(int index);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool setSelectedButton(int selectedButton, App * app);
    int selectedButton();
    ViewController * mainViewController() const;
    HeaderViewDelegate * headerViewDelegate() const;
  private:
    constexpr static KDCoordinate k_headerHeight = 20;
    constexpr static KDColor k_separatorHeaderColor = KDColor::RGB24(0xDEE0E2);
    constexpr static KDColor k_selectedBackgroundColor = KDColor::RGB24(0x426DA7);
    ViewController * m_mainViewController;
    int m_selectedButton;
    HeaderViewDelegate * m_delegate;
  };
  ContentView m_contentView;
};

#endif
