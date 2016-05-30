#include <escher/responder.h>

Responder::Responder() :
  m_parentResponder(nullptr)
{
}

Responder * Responder::parentResponder() {
  return m_parentResponder;
}

void Responder::setParentResponder(Responder * responder) {
  m_parentResponder = responder;
}

bool Responder::handleEvent(ion_event_t event) {
  return false;
}

void Responder::setFocused(bool focused) {
}
