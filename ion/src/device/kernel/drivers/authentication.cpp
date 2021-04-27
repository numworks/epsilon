#include <kernel/drivers/authentication.h>
#include <drivers/led.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/trampoline.h>
#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

typedef bool (*AuthenticationFunction)(void * pointer, uint32_t size);

bool userlandTrust(bool slotA) {
  uint32_t kernelAddress = slotA ? Board::Config::KernelAStartAddress : Board::Config::KernelBStartAddress;
   AuthenticationFunction * trampolineFunction = reinterpret_cast<AuthenticationFunction *>(Trampoline::addressOfFunction(TRAMPOLINE_AUTHENTICATION));
  return (*trampolineFunction)(reinterpret_cast<void *>(kernelAddress), Board::Config::KernelSize);
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
