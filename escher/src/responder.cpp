#include <escher/responder.h>
#include <escher/app.h>
#include <escher/toolbox.h>
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
  if (event == Ion::Events::Toolbox && toolbox() != nullptr) {
    toolbox()->setSender(this);
    app()->displayModalViewController(toolbox(), 0.f, 0.f, 50, 50, 0, 50);
    return true;
  }
  return false;
}

void Responder::didBecomeFirstResponder() {
}

void Responder::willResignFirstResponder() {
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

Toolbox * Responder::toolbox() {
  return nullptr;
}
