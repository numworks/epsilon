#ifndef ESCHER_APP_H
#define ESCHER_APP_H

#include <escher/editable_fiel_help_box.h>
#include <escher/i18n.h>
#include <escher/image.h>
#include <escher/modal_view_controller.h>
#include <escher/responder.h>
#include <escher/timer.h>
#include <escher/view_controller.h>
#include <escher/warning_controller.h>
#include <ion/storage/file_system.h>
#include <poincare/old/context.h>

/* An app is fed events and outputs drawing calls.
 *
 * To achieve this, it uses a View hierarchy, and modifies it according to the
 * events received. The View hierarchy keeps a memory of the area that needs to
 * be redrawn depending on how it's been modified.
 *
 * Multiple App can exist at once.
 * */

class AppsContainer;

namespace Escher {

class Container;
class EditableField;

class App : public Responder {
 public:
  class Descriptor {
   public:
    virtual I18n::Message name() const { return static_cast<I18n::Message>(0); }
    virtual I18n::Message upperName() const {
      return static_cast<I18n::Message>(0);
    }
    virtual const Image* icon() const { return nullptr; }
  };

  /* The Snapshot is the part of the App that is never destroyed, even when the
   * App is closed.
   *
   * IMPORTANT: When adding a variable to the Snapshot the functions `reset` and
   * `tidy` might need to be updated.
   *
   * - The `reset` function is called when all snapshot's variables need to be
   * set to their initial values (when the exam mode is activated for example).
   *
   * - The `tidy` function is called each time the app is closed, in case some
   * values in the Snapshot need to be cleaned (mainly to clean the pool).
   * */
  class Snapshot {
   public:
    virtual App* unpack(Container* container) = 0;
    void pack(App* app);
    /* reset all instances to their initial values */
    virtual void reset() {}
    /* tidy clean all dynamically-allocated data */
    virtual void tidy() {}
    virtual const Descriptor* descriptor() const = 0;
#if EPSILON_GETOPT
    virtual void setOpt(const char* name, const char* value) {}
#endif
    virtual void countryWasUpdated() {}
  };

  /* The destructor has to be virtual. Otherwise calling a destructor on an
   * App * pointing to a Derived App would have undefined behaviour. */
  virtual ~App() = default;
  Snapshot* snapshot() const { return m_snapshot; }
  void setFirstResponder(Responder* responder, bool force = false);
  Responder* firstResponder() { return m_firstResponder; }
  virtual bool processEvent(Ion::Events::Event event);
  void displayModalViewController(ViewController* vc, float verticalAlignment,
                                  float horizontalAlignment,
                                  KDMargins margins = {},
                                  bool growingOnly = false);
  void displayWarning(I18n::Message warningMessage,
                      bool specialExitKeys = false);

  virtual void didBecomeActive(Window* window);
  virtual void willBecomeInactive();
  View* modalView() { return m_modalViewController.view(); }
  ModalViewController* modalViewController() { return &m_modalViewController; }
  virtual void willOpenPage(ViewController* controller) {}
  virtual void didExitPage(ViewController* controller) {}
  virtual bool canStoreLayout() { return false; }
  virtual void storeLayout(Poincare::Layout layout = {}) {}
  virtual int numberOfTimers() { return 0; }
  virtual Timer* timerAtIndex(int i) {
    assert(false);
    return nullptr;
  }
  virtual Poincare::Context* localContext() { return nullptr; }
  virtual EditableFieldHelpBox* toolbox() { return nullptr; }
  virtual EditableFieldHelpBox* variableBox() { return nullptr; }

  virtual bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const {
    return true;
  }
  virtual void storageDidChangeForRecord(Ion::Storage::Record) {}

  static App* app();

 protected:
  App(Snapshot* snapshot, ViewController* rootViewController)
      : Responder(nullptr),
        m_modalViewController(this, rootViewController),
        m_firstResponder(this),
        m_snapshot(snapshot),
        m_warningController(this) {}
  ModalViewController m_modalViewController;

 private:
  Responder* m_firstResponder;
  Snapshot* m_snapshot;
  WarningController m_warningController;
};

}  // namespace Escher

#endif
