#ifndef ESCHER_APP_H
#define ESCHER_APP_H

#include <escher/modal_view_controller.h>
#include <escher/image.h>
#include <escher/i18n.h>
#include <escher/responder.h>
#include <escher/timer.h>
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
    virtual I18n::Message name();
    virtual I18n::Message upperName();
    virtual const Image * icon();
  };
  class Snapshot {
  public:
    virtual App * unpack(Container * container) = 0;
    void pack(App * app);
    /* reset all instances to their initial values */
    virtual void reset();
    virtual Descriptor * descriptor() = 0;
#if EPSILON_GETOPT
    virtual void setOpt(const char * name, char * value) {}
#endif
  private:
    /* tidy clean all dynamically-allocated data */
    virtual void tidy();
  };
  virtual ~App() = default;
  constexpr static uint8_t Magic = 0xA8;
  Snapshot * snapshot();
  void setFirstResponder(Responder * responder);
  Responder * firstResponder();
  virtual bool processEvent(Ion::Events::Event event);
  void displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin = 0, KDCoordinate leftMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate rightMargin = 0);
  void dismissModalViewController();
  void displayWarning(I18n::Message warningMessage);
  const Container * container() const;
  uint8_t m_magic; // Poor man's RTTI

  virtual void didBecomeActive(Window * window);
  virtual void willBecomeInactive();
  View * modalView();
  virtual int numberOfTimers();
  virtual Timer * timerAtIndex(int i);
protected:
  App(Container * container, Snapshot * snapshot, ViewController * rootViewController, I18n::Message warningMessage = (I18n::Message)0);
  ModalViewController m_modalViewController;
private:
  Container * m_container;
  Responder * m_firstResponder;
  Snapshot * m_snapshot;
  WarningController m_warningController;
};

#endif

