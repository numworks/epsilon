#ifndef ESCHER_APP_H
#define ESCHER_APP_H

#include <escher/modal_view_controller.h>
#include <escher/image.h>
#include <escher/i18n.h>
#include <escher/responder.h>
#include <escher/view_controller.h>
#include <escher/warning_controller.h>

/* An app is fed events and outputs drawing calls.
 *
 * To achieve this, it uses a View hierarchy, and modifies it according to the
 * events received. The View hierarchy keeps a memory of the area that needs to
 * be redrawn depending on how it's been modified.
 *
 * Multiple App can exist at once.
 * */

class Container;

class App : public Responder {
public:
  class Descriptor {
  public:
    virtual App * build(Container * container) = 0;
    virtual I18n::Message name();
    virtual I18n::Message upperName();
    virtual const Image * icon();
  };
  /* Each App subclass must implement
   * static Descriptor * builDescriptor(); */
  Descriptor * descriptor();
  virtual ~App() = default;
  constexpr static uint8_t Magic = 0xA8;
  void setFirstResponder(Responder * responder);
  Responder * firstResponder();
  bool processEvent(Ion::Events::Event event);
  void displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin = 0, KDCoordinate leftMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate rightMargin = 0);
  void dismissModalViewController();
  void displayWarning(I18n::Message warningMessage);
  const Container * container() const;
  uint8_t m_magic; // Poor man's RTTI

  virtual void didBecomeActive(Window * window);
  virtual void willBecomeInactive();
protected:
  App(Container * container, ViewController * rootViewController, Descriptor * descriptor, I18n::Message warningMessage = (I18n::Message)0);
  ModalViewController m_modalViewController;
private:
  Descriptor * m_descriptor;
  Container * m_container;
  Responder * m_firstResponder;
  WarningController m_warningController;
};

#endif

