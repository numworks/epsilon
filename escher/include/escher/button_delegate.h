#ifndef ESCHER_ABSTRACT_BUTTON_DELEGATE_H
#define ESCHER_ABSTRACT_BUTTON_DELEGATE_H

#include <escher/invocation.h>

namespace Escher {

/* Simple Delegate interface to use a button
 * Just need to define buttonAction() */
// TODO use everywhere
class ButtonDelegate {
public:
  virtual bool buttonAction() = 0;
  Invocation buttonActionInvocation() {
    return Invocation(
        [](void * c, void * s) {
          return static_cast<ButtonDelegate *>(c)->buttonAction();
        },
        this);
  }
};

}  // namespace Escher

#endif /* ESCHER_ABSTRACT_BUTTON_DELEGATE_H */
