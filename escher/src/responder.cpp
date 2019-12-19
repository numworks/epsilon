#include <escher/responder.h>
#include <escher/container.h>
#include <assert.h>


Responder * Responder::commonAncestorWith(Responder * responder) {
  if (responder == nullptr) {
    return nullptr;
  }
  Responder * p = this;
  while (p != nullptr) {
    if (responder->hasAncestor(p)) {
      return p;
    }
    p = p->parentResponder();
  }
  return nullptr;
}

bool Responder::hasAncestor(Responder * responder) const {
  assert(responder != nullptr);
  Responder * p = const_cast<Responder *>(this);
  while (p != nullptr) {
    if (p == responder) {
      return true;
    }
    p = p->parentResponder();
  }
  return false;
}
