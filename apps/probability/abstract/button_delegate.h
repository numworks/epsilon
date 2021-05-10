#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <escher/invocation.h>

class ButtonDelegate {
 public:
  virtual void buttonAction() = 0;

 protected:
  Escher::Invocation buttonActionInvocation() {
    return Escher::Invocation(
        [](void * c, void * s) {
          static_cast<ButtonDelegate *>(c)->buttonAction();
          return true;
        },
        this);
  }
};

#endif /* BUTTON_CONTROLLER_H */
