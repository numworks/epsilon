#include <escher/responder.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

void Responder::modalViewAltersFirstResponder(FirstResponderAlteration alteration) {
  privateModalViewAltersFirstResponder(alteration);
  if (m_parentResponder) {
    m_parentResponder->modalViewAltersFirstResponder(alteration);
  }
}

bool Responder::hasAncestor(Responder * responder) const {
  if (responder == nullptr) {
    return false;
  }
  return privateHasAncestor(responder);
}

Responder * Responder::commonAncestorWith(Responder * responder) {
  if (responder == nullptr) {
    return nullptr;
  }
  Responder * p = this;
  while (p != nullptr) {
    if (responder->privateHasAncestor(p)) {
      return p;
    }
    p = p->parentResponder();
  }
  return nullptr;
}

bool Responder::privateHasAncestor(Responder * responder) const {
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

}
