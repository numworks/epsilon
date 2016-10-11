#ifndef HEADER_VIEW_CONTROLLER_H
#define HEADER_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/text_view.h>
#include <escher/childless_view.h>

class HeaderViewController : public ViewController {
public:
  HeaderViewController(Responder * parentResponder, View * mainView);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setVisibleHeader(bool isVisibleHeader);
protected:
  void setButtonTitles(const char * buttonOneTitle, const char * buttonTwoTitle, const char * buttonThreeTitle);
  void setSelectedButton(int selectedButton);
private:
  class ContentView : public View {
  public:
    ContentView(View * subview);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setVisibleHeader(bool isVisibleHeader);
    void setButtonTitles(const char * buttonOneTitle, const char * buttonTwoTitle, const char * buttonThreeTitle);
    void setSelectedButton(int selectedButton);
    int selectedButton();
  private:
    constexpr static KDCoordinate k_headerHeight = 20;
    constexpr static KDCoordinate k_buttonTextMargin = 10;
    constexpr static KDColor k_separatorHeaderColor = KDColor(0xDEE0E2);
    constexpr static KDColor k_selectedBackgroundColor = KDColor(0x426DA7);;
    TextView m_buttonOne;
    TextView m_buttonTwo;
    TextView m_buttonThree;
    int m_numberOfButtons;
    View * m_mainView;
    bool m_visibleHeader;
    int m_selectedButton;
  };
  ContentView m_contentView;

};

#endif
