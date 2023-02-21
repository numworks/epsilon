#ifndef ELEMENTS_MAIN_CONTROLLER_H
#define ELEMENTS_MAIN_CONTROLLER_H

#include <escher/stack_view_controller.h>

#include "banner_view.h"
#include "details_list_controller.h"
#include "display_type_controller.h"
#include "elements_view.h"
#include "elements_view_delegate.h"

namespace Elements {

class MainController : public Escher::ViewController,
                       public ElementsViewDelegate,
                       public Escher::TextFieldDelegate {
 public:
  MainController(Escher::StackViewController* parentResponder);

  // Escher::ViewController
  Escher::View* view() override { return &m_view; }
  void viewWillAppear() override { m_view.elementsView()->dirtyBackground(); }

  // Escher::Responder
  void didBecomeFirstResponder() override {
    Escher::Container::activeApp()->setFirstResponder(
        m_view.bannerView()->textField());
  }
  bool handleEvent(Ion::Events::Event e) override;

  // ElementsViewDelegate
  void selectedElementHasChanged() override;
  void activeDataFieldHasChanged() override;

  // Escher::TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  void textFieldDidStartEditing(Escher::AbstractTextField* textField) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::AbstractTextField* textField) override;
  bool textFieldDidHandleEvent(Escher::AbstractTextField* textField,
                               bool returnValue, bool textDidChange) override;

 private:
  class ContentView : public Escher::View {
   public:
    ContentView(MainController* mainController)
        : m_bannerView(mainController, mainController) {}

    ElementsView* elementsView() { return &m_elementsView; }
    BannerView* bannerView() { return &m_bannerView; }

   private:
    int numberOfSubviews() const override { return 2; }
    Escher::View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    ElementsView m_elementsView;
    BannerView m_bannerView;
  };

  void endElementSearch(AtomicNumber z);

  DetailsListController m_detailsController;
  DisplayTypeController m_displayTypeController;
  ContentView m_view;
};

}  // namespace Elements

#endif
