#include <kernel/drivers/config/board.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/cortex_control.h>
#include <kernel/drivers/timing.h>
#include <kernel/warning_display.h>
#include <shared/drivers/backlight.h>
#include <shared/drivers/display.h>
#include <ion/display.h>
#include <kandinsky/font.h>
#include <string.h>

extern "C" {
  extern char _kernel_start;
}

typedef void (*EntryPoint)();

void kernel_main() {
  // TODO: move this code after a DFU leave. There is no reason to execute a untrusted userland after a reset
  // Display warning for unauthenticated software
  if (!Ion::Device::Authentication::trustedUserland()) {
    Ion::Device::Backlight::init();
    Ion::Device::WarningDisplay::unauthenticatedUserland();
    Ion::Device::Timing::msleep(5000);
    Ion::Device::Backlight::shutdown();
  }
  // Unprivileged mode
  switch_to_unpriviledged();

  // Jump to userland
  EntryPoint * userlandFirstAddress = reinterpret_cast<EntryPoint *>(&_kernel_start + Ion::Device::Board::Config::UserlandOffsetFromKernel);
  (*userlandFirstAddress)();
}
