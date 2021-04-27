#include <kernel/drivers/board.h>
#include <drivers/reset.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/cortex_control.h>
#include <assert.h>
#include <string.h>

void kernel_main() {
  assert(Ion::Device::Authentication::trustedUserland());

  switch_to_unpriviledged();

  // Jump to userland
  Ion::Device::Reset::jump(Ion::Device::Board::userlandStart(), false);
}
