#include <drivers/circuit_breaker.h>
#include <drivers/cache.h>
#include <kernel/boot/isr.h>
#include <regs/regs.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


namespace Ion {
namespace Device {
namespace Checkpoint {

/* Basic frame keeps up to 0x24 bytes */
static constexpr size_t k_basicFrameSize = 0x20;
static constexpr size_t k_extendedFrameSize = 0x68;
static constexpr size_t k_exceptionStackMaxSize = 100;
uint8_t sExceptionStack[k_extendedFrameSize + k_exceptionStackMaxSize];
uint8_t * sExceptionStackAddress = nullptr;
size_t sExceptionStackSize;
jmp_buf sRegisters;
//static constexpr size_t k_numberOfCalleeSaveRegisters = 26;
//uint32_t sCalleeSaveRegisters[k_numberOfCalleeSaveRegisters];
//uint32_t sExcReturn = 0;

size_t frameSize(uint32_t excReturn) {
  bool useBasicFrame = excReturn & 0x10;
  return useBasicFrame ? k_basicFrameSize : k_extendedFrameSize;
}

bool hasCheckpoint() {
  return sExceptionStackAddress != nullptr;
}

void setCheckpoint(uint8_t * frameAddress, uint32_t excReturn) {
  if (hasCheckpoint()) {
    // Keep the oldest checkpoint
    return; //false;
  }
  // Disable other exceptions or change priority!
  /* Step 1: get exception return value indicating the return context:
   * - the stack frame used (FPU extended?),
   * - the stack pointer used (MSP/PSP)
   * - the mode used (thread/handler) */
  /*NOTE: On exception entry, the ARM hardware uses bit 9 of the stacked xPSR value to indicate whether 4 bytes of padding was added to align the stack on an 8 byte boundary.
   * using the STKALIGN bit of the Configuration Control Register (CCR).
   * In the Cortex-M7 processor CCR.STKALIGN is read-only and has a value of 1. This means that the exception stack frame starting address is always 8-byte aligned.
   * assert CCR.STKALIGN == 1?*/
  // TODO: alignement issue!!!

  // Extract current stack pointer
  uint8_t * stackPointerAddress = nullptr;
  // Copy sp address
  asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);
  // Store the stackframe
  sExceptionStackAddress = stackPointerAddress;
  sExceptionStackSize = frameAddress + frameSize(excReturn) - stackPointerAddress;
  memcpy(sExceptionStack, sExceptionStackAddress, sExceptionStackSize);
  // Store the callee-save registers (which aren't saved in the frame)
  /*asm volatile (
      // Store registers we want to use
      "push {r0,ip}                  \t\n"
      // Copy stack pointer in a scratch reg
      "mov ip, sp                   \t\n"
      // ip stores stack pointers before pushing r0/ip
      "add ip, 0x8                   \t\n"
      // Save all the callee-save registers in buffer
      "mov r0,%[regsBufferAddress]   \t\n"
      // storing r4-r9, r10=sl, r11=fp, ip=sp, lr
      "stmia r0!,{r4-r11,ip,lr}      \t\n"
      // storing floating point registers
      "vstmia r0!, {s16-s31}         \t\n"
      // Restore registers
      "pop {r0-ip}         \t\n"
      : :
      [regsBufferAddress] "r" (sCalleeSaveRegisters)
  );*/

  if (setjmp(sRegisters)) {
    // Restore frame
    memcpy(sExceptionStackAddress, sExceptionStack, sExceptionStackSize);
    // Reset checkpoint
    unsetCheckpoint();
    //return true;
  }
  //return false;
}

void unsetCheckpoint() {
  sExceptionStackAddress = nullptr;
}

void loadCheckpoint(uint8_t * frameAddress) {
  // TODO: priority + alignement issue!!!
  // Disable other exceptions or change priority!
  assert(hasCheckpoint());

  // Restore callee save registers (which aren't saved in the frame)
  /*asm volatile (
      // Store registers we want to use
      "push {r0,ip}                  \t\n"
      // Save all the callee-save registers in buffer
      "mov r0,%[regsBufferAddress]   \t\n"
      // loading r4-r9, r10=sl, r11=fp, ip=sp, lr
      "ldmia r0!,{r4-r11,ip,lr}      \t\n"
      // loading floating point registers
      "vldmia r0!, {s16-s31}         \t\n"
      // Restore registers
      "pop {r0,ip}                  \t\n"
      // Restore stack pointer
      "ldr sp,[sp,-0x08]            \t\n"
      // Update stack pointer address
      "mov sp,%[stackPointerAddress]   \t\n"
      :
      [stackPointerAddress] "=r" (frameAddress)
      :
      [regsBufferAddress] "r" (sCalleeSaveRegisters)
  );*/



  // Restore registers
  longjmp(sRegisters, 1);
}

}
}
}
