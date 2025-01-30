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
