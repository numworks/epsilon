#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion/events.h>

namespace Escher {

class Responder {
 public:
  Responder(Responder* parentResponder) : m_parentResponder(parentResponder) {}
  // Default implementation does nothing
  virtual bool handleEvent(Ion::Events::Event event) { return false; };

  void didBecomeFirstResponder() {
    handleResponderChainEvent(
        {{nullptr}, ResponderChainEventType::BecameFirst});
  }
  void willResignFirstResponder() {
    handleResponderChainEvent(
        {{nullptr}, ResponderChainEventType::WillResignFirst});
  }

  void didEnterResponderChain(Responder* previousFirstResponder) {
    handleResponderChainEvent(
        {{previousFirstResponder}, ResponderChainEventType::DidEnter});
  }
  void willExitResponderChain(Responder* nextFirstResponder) {
    handleResponderChainEvent(
        {{nextFirstResponder}, ResponderChainEventType::WillExit});
  }

  /* This struct was created to reduce the size of the vtables of Responder and
   * descendants, with this, we only have a single virtual class:
   * handleResponderChainEvent, and the 4 methods above are no longer overridden
   * by each children. Also the union allow writing event.nextFirstResponder
   * when reacting to a WillExit event and event.previousFirstResponder when
   * DidEnter : this allow easier readability of the code */
  enum class ResponderChainEventType {
    DidEnter,
    WillExit,
    WillResignFirst,
    BecameFirst
  };
  struct ResponderChainEvent {
    union {
      Responder* nextFirstResponder;
      Responder* previousFirstResponder;
    };
    ResponderChainEventType type;
  };

  enum class FirstResponderAlteration { WillSpoil, DidRestore };
  void modalViewAltersFirstResponder(FirstResponderAlteration alteration);

  Responder* parentResponder() const { return m_parentResponder; }
  bool hasAncestor(Responder* responder) const;
  Responder* commonAncestorWith(Responder* responder);
  void setParentResponder(Responder* responder) {
    m_parentResponder = responder;
  }

 protected:
  virtual void handleResponderChainEvent(ResponderChainEvent event) {}

 private:
  bool privateHasAncestor(Responder* responder) const;
  virtual void privateModalViewAltersFirstResponder(
      FirstResponderAlteration alteration) {}

  Responder* m_parentResponder;
};

}  // namespace Escher
#endif
