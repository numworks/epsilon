#include "main.h"

namespace Ion {
namespace Simulator {
namespace FXCGMenuHandler {

int saveAndOpenMainMenu(void) {
  int addr;

  // get the address of the syscall table in it
  addr = *(unsigned int *)0x8002007C;

  if (addr < (int)0x80020070)
    return 1;
  if (addr >= (int)0x81000000)
    return 1;

  // get the pointer to syscall 1E58 - SwitchToMainMenu
  addr += 0x1E58 * 4;
  if (addr < (int)0x80020070)
    return 1;
  if (addr >= (int)0x81000000)
    return 1;

  addr = *(unsigned int *)addr;
  if (addr < (int)0x80020070)
    return 1;
  if (addr >= (int)0x81000000)
    return 1;

  // Now addr has the address of the first operation in %1e58

  // Run up to 150 times (300/2). OS 3.60's is 59 instructions, so this should
  // be plenty, but will let it stop if nothing is found
  for (unsigned short *currentAddr = (unsigned short *)addr;
       (unsigned int)currentAddr < ((unsigned int)addr + 300); currentAddr++) {
    // MOV.L GetkeyToMainFunctionReturn Flag, r14
    if (*(unsigned char *)currentAddr != 0xDE)
      continue;

    // MOV #3, 2
    if (*(currentAddr + 1) != 0xE203)
      continue;

    // BSR <SaveAndOpenMainMenu>
    if ((*(unsigned char *)(currentAddr + 2) & 0xF0) != 0xB0)
      continue;

    // MOV.B r2, @r14
    if (*(currentAddr + 3) != 0x2E20)
      continue;

    // BRA <some addr>
    if ((*(unsigned char *)(currentAddr + 4) & 0xF0) != 0xA0)
      continue;

    // NOP
    if (*(currentAddr + 5) != 0x0009)
      continue;

    unsigned short branchInstruction = *(currentAddr + 2);

    // Clear first 4 bits (BSR identifier)
    branchInstruction <<= 4;
    branchInstruction >>= 4;

    // branchInstruction is now the displacement of BSR

    // Create typedef so we can cast the pointer
    typedef void (*voidFunc)(void);

    // JMP to disp*2 + PC + 4
    ((voidFunc)((unsigned int)branchInstruction * 2 +
                (unsigned int)currentAddr + 4 + 4))();

    return 0;
  }

  return 1;
}

extern "C" void gint_osmenu_native(void);

void openMenuWrapper(void) {
  if (saveAndOpenMainMenu() != 0) {
    // Fallback
    gint_osmenu_native();
  }
}

void openMenu(void) {
  Simulator::Main::runPowerOffSafe(openMenuWrapper, true);
}

}
}
}
