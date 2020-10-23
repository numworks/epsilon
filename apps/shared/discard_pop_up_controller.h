#ifndef SHARED_DISCARD_POP_UP_CONTROLLER_H
#define SHARED_DISCARD_POP_UP_CONTROLLER_H

#include <escher/pop_up_controller.h>

namespace Shared {

class DiscardPopUpController : public Escher::PopUpController {
public:
  DiscardPopUpController(Escher::Invocation OkInvocation);
};

}

#endif
