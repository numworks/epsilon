#ifndef ESCHER_APP_H
#define ESCHER_APP_H

#include <escher/modal_view_controller.h>
#include <escher/responder.h>
#include <escher/view_controller.h>
#include <escher/image.h>
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
  constexpr static uint8_t Magic = 0xA8;
  App(Container * container, ViewController * rootViewController, const char * name = nullptr, const Image * icon = nullptr);
  void setWindow(Window * window);
  void setFirstResponder(Responder * responder);
  Responder * firstResponder();
  void processEvent(Ion::Events::Event event);
  void displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin = 0, KDCoordinate leftMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate rightMargin = 0);
  void dismissModalViewController();
  void displayWarning(const char * warningMessage);
  const Container * container() const;
  const char * name();
  const Image * icon();
  uint8_t m_magic; // Poor man's RTTI
protected:
  ModalViewController m_modalViewController;
private:
  Container * m_container;
  Responder * m_firstResponder;
  WarningController m_warningController;
  const char * m_name;
  const Image * m_icon;
};

#endif
