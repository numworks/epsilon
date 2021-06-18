#ifndef ESCHER_APP_H
#define ESCHER_APP_H

#include <escher/modal_view_controller.h>
#include <escher/image.h>
#include <escher/i18n.h>
#include <escher/responder.h>
#include <escher/timer.h>
#include <escher/view_controller.h>
#include <escher/warning_controller.h>
#include <ion/storage.h>
#include <poincare/context.h>
#include <escher/telemetry.h>

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
    virtual I18n::Message name() { return (I18n::Message)0; }
    virtual I18n::Message upperName() { return (I18n::Message)0; }
    enum class ExaminationLevel {
      No= 0,
      Basic = 1,
      Strict = 2,
    };
    virtual ExaminationLevel examinationLevel();
    virtual const Image * icon() { return nullptr; }
  };
  class Snapshot {
  public:
    virtual App * unpack(Container * container) = 0;
    virtual void pack(App * app);
    /* reset all instances to their initial values */
    virtual void reset() {}
    virtual void storageDidChangeForRecord(Ion::Storage::Record) {}
    virtual Descriptor * descriptor() = 0;
#if EPSILON_GETOPT
    virtual void setOpt(const char * name, const char * value) {}
#endif
    /* tidy clean all dynamically-allocated data */
    virtual void tidy() {}
  };
  /* The destructor has to be virtual. Otherwise calling a destructor on an
   * App * pointing to a Derived App would have undefined behaviour. */
  virtual ~App() = default;
  Snapshot * snapshot() const { return m_snapshot; }
  void setFirstResponder(Responder * responder);
  Responder * firstResponder() { return m_firstResponder; }
  virtual bool processEvent(Ion::Events::Event event);
  /* prepareForExit returns true if the app can be switched off in the current
   * runloop step, else it prepares for a switch off and returns false. */
  virtual bool prepareForExit() { return true; }
  void displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin = 0, KDCoordinate leftMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate rightMargin = 0);
  void dismissModalViewController(bool willExitApp = false);
  void displayWarning(I18n::Message warningMessage1, I18n::Message warningMessage2 = (I18n::Message) 0, bool specialExitKeys = false);

  virtual void didBecomeActive(Window * window);
  virtual void willBecomeInactive();
  View * modalView();
  virtual int numberOfTimers() { return 0; }
  virtual Timer * timerAtIndex(int i) { assert(false); return nullptr; }
  virtual Poincare::Context * localContext() { return nullptr; }
protected:
  App(Snapshot * snapshot, ViewController * rootViewController, I18n::Message warningMessage = (I18n::Message)0) :
    Responder(nullptr),
    m_modalViewController(this, rootViewController),
    m_firstResponder(nullptr),
    m_snapshot(snapshot),
    m_warningController(this, warningMessage)
  {}
  ModalViewController m_modalViewController;
private:
  Responder * m_firstResponder;
  Snapshot * m_snapshot;
  WarningController m_warningController;
};

#endif

