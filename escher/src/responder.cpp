#include <escher/responder.h>
#include <escher/container.h>
#include <assert.h>

Responder::Responder(Responder * parentResponder) :
  m_parentResponder(parentResponder)
{
}

Responder * Responder::parentResponder() const {
  return m_parentResponder;
}

void Responder::setParentResponder(Responder * responder) {
  m_parentResponder = responder;
}

void Responder::didBecomeFirstResponder() {
}

void Responder::willResignFirstResponder() {
}

void Responder::didEnterResponderChain(Responder * previousFirstResponder) {
}

void Responder::willExitResponderChain(Responder * nextFirstResponder) {
}

Responder * Responder::commonAncestorWith(Responder * responder) {
  if (responder == nullptr) {
    return nullptr;
  }
  if (this == responder) {
    return this;
  }
  Responder * rootResponder = this;
  while (rootResponder->parentResponder() != responder && rootResponder->parentResponder() != nullptr) {
    rootResponder = rootResponder->parentResponder();
  }
  if (rootResponder->parentResponder() == responder) {
    return responder;
  }
  rootResponder = responder;
  while (rootResponder->parentResponder() != this && rootResponder->parentResponder() != nullptr) {
    rootResponder = rootResponder->parentResponder();
  }
  if (rootResponder->parentResponder() == this) {
    return this;
  }
  Responder * r = nullptr;
  if (parentResponder()) {
    r = parentResponder()->commonAncestorWith(responder);
   }
   Responder * s = nullptr;
   if (responder->parentResponder()) {
    s = commonAncestorWith(responder->parentResponder());
   }
   if (r) {
    return r;
  }
  return s;
}

App * Responder::app() const {
  return Container::sharedContainer()->activeApp();
}
