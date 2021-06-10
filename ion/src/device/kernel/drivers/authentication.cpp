#include <kernel/drivers/authentication.h>
#include <drivers/led.h>
#include <kernel/drivers/board.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/trampoline.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

void updateTrust(bool trust) {
  s_trustedUserland = trust;
  if (!s_trustedUserland) {
    LED::setColor(KDColorBlack);
    // Bloquer les options bytes? Mettre tout le running SLOT en RDP WRP sauf exam mode?
  }
}

}
}
}
