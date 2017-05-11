#ifndef ON_BOARDING_UPDATE_CONTROLLER_H
#define ON_BOARDING_UPDATE_CONTROLLER_H

#include <escher.h>
#include "../shared/ok_view.h"

class UpdateController : public ViewController {
public:
  UpdateController();
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  class ContentView : public View {
  public:
    ContentView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    constexpr static KDCoordinate k_titleMargin = 40;
    constexpr static KDCoordinate k_paragraphHeight = 100;
    constexpr static KDCoordinate k_paragraphMargin = 13;
    constexpr static KDCoordinate k_bottomMargin = 13;
    constexpr static KDCoordinate k_okMargin = 10;
    constexpr static KDCoordinate k_skipMargin = 4;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_titleTextView;
    MessageTextView m_messageTextView1;
    MessageTextView m_messageTextView2;
    MessageTextView m_messageTextView3;
    MessageTextView m_messageTextView4;
    MessageTextView m_skipView;
    Shared::OkView m_okView;
  };
  ContentView m_contentView;
};

#endif

