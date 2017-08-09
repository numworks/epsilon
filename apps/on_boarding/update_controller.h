#ifndef ON_BOARDING_UPDATE_CONTROLLER_H
#define ON_BOARDING_UPDATE_CONTROLLER_H

#include "../shared/message_controller.h"

namespace OnBoarding {

class UpdateController : public MessageController {
public:
  UpdateController();
  bool handleEvent(Ion::Events::Event event) override;
};

}

#endif

