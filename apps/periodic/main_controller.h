#ifndef PERIODIC_MAIN_CONTROLLER_H
#define PERIODIC_MAIN_CONTROLLER_H

#include "banner_view.h"
#include "elements_view.h"
#include <escher/view_controller.h>

namespace Periodic {

class MainController : public Escher::ViewController {
public:
  MainController(Escher::Responder * parentResponder, ElementsViewDataSource * dataSource) : ViewController(parentResponder), m_view(dataSource) {}

  // Escher::ViewController
  Escher::View * view() override { return &m_view; }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event e) override;

private:
  class ContentView : public Escher::View {
  public:
    ContentView(ElementsViewDataSource * dataSource) : m_elementsView(dataSource), m_bannerView(dataSource) {}
    ElementsView * elementsView() { return &m_elementsView; }
    BannerView * bannerView() { return &m_bannerView; }

  private:
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    ElementsView m_elementsView;
    BannerView m_bannerView;
  };

  ContentView m_view;
};

}

#endif

