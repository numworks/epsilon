#include <kernel/drivers/authentication.h>
#include <drivers/led.h>
#include <kernel/drivers/board.h>
#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Authentication {

static Ion::Authentication::ClearanceLevel s_clearanceLevel = Ion::Authentication::ClearanceLevel::NumWorks;

Ion::Authentication::ClearanceLevel clearanceLevel() {
  return s_clearanceLevel;
}

void downgradeClearanceLevelTo(Ion::Authentication::ClearanceLevel level) {
  assert(static_cast<int>(level) > static_cast<int>(s_clearanceLevel));
  LED::setColorSecure(KDColorBlack);
  s_clearanceLevel = level;
  assert(s_clearanceLevel != Ion::Authentication::ClearanceLevel::NumWorks);
  // Bloquer les options bytes? Mettre tout le running SLOT en RDP WRP sauf exam mode?
}

}
}
}
