#ifndef SOLVER_ABSTRACT_BUTTON_DELEGATE_H
#define SOLVER_ABSTRACT_BUTTON_DELEGATE_H

#include <escher/invocation.h>

namespace Solver {

/* Simple Delegate interface to use a button
 * Just need to define buttonAction() */
// TODO use everywhere
class ButtonDelegate {
public:
  virtual bool buttonAction() = 0;
  Escher::Invocation buttonActionInvocation() {
    return Escher::Invocation(
        [](void * c, void * s) {
          return static_cast<ButtonDelegate *>(c)->buttonAction();
        },
        this);
  }
};

}  // namespace Solver

#endif /* SOLVER_ABSTRACT_BUTTON_DELEGATE_H */
