#include <escher/responder.h>
#include <escher/app.h>
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

bool Responder::handleEvent(Ion::Events::Event event) {
  return false;
}

void Responder::didBecomeFirstResponder() {
}

void Responder::didResignFirstResponder() {
}

/* We assume the app is the root parent. */
App * Responder::app() {
  Responder * rootResponder = this;
  while (rootResponder->parentResponder() != nullptr) {
    rootResponder = rootResponder->parentResponder();
  }
  /* If we used RTTI we could use a dynamic_cast, which would be a lot more
   * safe, as such:
   * return dynamic_cast<App *>(rootResponder); */
   App * result = (App *)rootResponder;
   assert(result->m_magic == App::Magic); // Poor man's RTTI
  return result;
}
