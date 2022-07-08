#ifndef PERIODIC_MAIN_CONTROLLER_H
#define PERIODIC_MAIN_CONTROLLER_H

#include "banner_view.h"
#include "coloring_choice_controller.h"
#include "elements_view.h"
#include "elements_view_delegate.h"
#include <escher/stack_view_controller.h>

namespace Periodic {

class MainController : public Escher::ViewController, public ElementsViewDelegate {
public:
  MainController(Escher::StackViewController * parentResponder) : ViewController(parentResponder), m_coloringController(parentResponder) {}

  // Escher::ViewController
  Escher::View * view() override { return &m_view; }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event e) override;

  // ElementsViewDelegate
  void selectedElementHasChanged(AtomicNumber oldZ) override;

private:
  class ContentView : public Escher::View {
  public:
    ElementsView * elementsView() { return &m_elementsView; }
    BannerView * bannerView() { return &m_bannerView; }

  private:
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    ElementsView m_elementsView;
    BannerView m_bannerView;
  };

  ColoringChoiceController m_coloringController;
  ContentView m_view;
  AtomicNumber m_previousElement;
};

}

#endif

