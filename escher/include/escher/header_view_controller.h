#ifndef HEADER_VIEW_CONTROLLER_H
#define HEADER_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/view.h>
#include <escher/invocation.h>
#include <escher/button.h>
#include <escher/app.h>

class HeaderViewController : public ViewController {
public:
  HeaderViewController(Responder * parentResponder, View * mainView);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setVisibleHeader(bool isVisibleHeader);
  virtual int numberOfButtons() const;
  virtual Button * buttonAtIndex(int index);
protected:
  void setSelectedButton(int selectedButton);
private:
  class ContentView : public View {
  public:
    ContentView(View * subview, HeaderViewController * headerViewController);
    int numberOfButtons() const;
    Button * buttonAtIndex(int index);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setVisibleHeader(bool isVisibleHeader);
    void setSelectedButton(int selectedButton, App * app);
    int selectedButton();
  private:
    constexpr static KDCoordinate k_headerHeight = 20;
    constexpr static KDColor k_separatorHeaderColor = KDColor(0xDEE0E2);
    constexpr static KDColor k_selectedBackgroundColor = KDColor(0x426DA7);;
    View * m_mainView;
    bool m_visibleHeader;
    int m_selectedButton;
    HeaderViewController * m_headerViewController;
  };
  ContentView m_contentView;

};

#endif
