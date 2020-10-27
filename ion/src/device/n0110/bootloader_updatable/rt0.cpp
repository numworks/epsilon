#include "isr.h"
#include <stdint.h>
#include <string.h>
#include <ion.h>
#include <drivers/backlight.h>
#include <drivers/board_privileged.h>
#include <drivers/cache.h>
#include <drivers/keyboard.h>
#include <drivers/reset.h>
#include <drivers/timing.h>
#include <regs/regs.h>
#include <kandinsky.h>

typedef void (*cxx_constructor)();

extern "C" {
  extern char _data_section_start_flash;
  extern char _data_section_start_ram;
  extern char _data_section_end_ram;
  extern char _bss_section_start_ram;
  extern char _bss_section_end_ram;
  extern cxx_constructor _init_array_start;
  extern cxx_constructor _init_array_end;
  extern void switch_to_unpriviledged();
}

void __attribute__((noinline)) abort() {
#ifdef NDEBUG
  Ion::Device::Reset::core();
#else
  while (1) {
  }
#endif
}

// TODO EMILIE: Duplicate

void decrypt(uint8_t * signature, uint8_t * decryptedSignature) {
  // TODO : Decrypt signature with public key
  memcpy(decryptedSignature, signature, Ion::Sha256DigestBytes);
}

// TODO EMILIE: Duplicate

bool IsAuthenticated(void * pointer) {
  constexpr uint32_t SizeOfExternalFlash = 8 * 1024 * 1024; // TODO EMILIE duplicated variable
  constexpr uint32_t SizeOfUpdatableBootloader = (64 - 16) * 1024; // TODO EMILIE duplicated variable
  constexpr uint32_t SizeOfExamModeSection = 4 * 1024;
  //| size | updatable bootloader | signature | size | exam mode | available external flash | signature |
  constexpr uint32_t SizeOfAvailableExternalFlash = SizeOfExternalFlash - SizeOfUpdatableBootloader - SizeOfExamModeSection - sizeof(uint32_t)*2 - 32*2; // 32 = sizeof(signature)

  /* Data structure at pointer must be :
   * | code size |         code        |   signature   | */
  // Extract size and code
  uint32_t size = *(uint32_t*) pointer;
  uint8_t * code = (uint8_t *)pointer + sizeof(uint32_t);
  if (size <= SizeOfExamModeSection || size >= SizeOfAvailableExternalFlash) {
    return false;
  }

  //TODO LEA FIXME FIXME Dirty trick to not sha the eam mode
  size -= SizeOfExamModeSection;
  code += SizeOfExamModeSection;


  // Hash code
  uint8_t digest[Ion::Sha256DigestBytes];
  // By construction, Sha256 also hashes code size into digest
  Ion::sha256(code, size, digest);
  // Extract and Decrypt signature
  uint8_t * signature = code + size;
  uint8_t decryptedSignature[Ion::Sha256DigestBytes];
  decrypt(signature, decryptedSignature);
  // Code is authenticated if signature decrypts to digest
  return memcmp(digest, decryptedSignature, Ion::Sha256DigestBytes) == 0;
}

bool ExternalFlashIsAuthenticated() {
  void * externalFlashEpsilonAddress = reinterpret_cast<void *>(0x9000C024); // TODO EMILIE no magic number
  return IsAuthenticated(externalFlashEpsilonAddress);
}

constexpr static int sNumberOfMessages = 7;
constexpr static const char * sMessages[sNumberOfMessages] = {
  "NON-AUTHENTICATED SOFTWARE",
  "Caution: you're using an",
  "unofficial software version.",
  "NumWorks can't be held responsible",
  "for any resulting damage.",
  "Some features (blue and green LED)",
  "are unavailable."
};

void displayWarningMessage() {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(screen, KDColorWhite);
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  KDCoordinate margin = 60;
  KDCoordinate currentHeight = 0;
  currentHeight += margin;
  const char * title = sMessages[0];
  KDSize titleSize = KDFont::LargeFont->stringSize(title);
  ctx->drawString(title, KDPoint((Ion::Display::Width-titleSize.width())/2, currentHeight), KDFont::LargeFont);
  currentHeight += 2*titleSize.height();
  for (int j = 1; j < sNumberOfMessages; j++) {
    const char * message = sMessages[j];
    KDSize messageSize = KDFont::SmallFont->stringSize(message);
    ctx->drawString(message, KDPoint((Ion::Display::Width-messageSize.width())/2, currentHeight), KDFont::SmallFont);
    currentHeight += messageSize.height();
  }
  Ion::Timing::msleep(5000);
}

void ColorScreen(uint32_t color) {
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(color));
  Ion::Timing::msleep(500);
}

/* This additional function call 'jump_to_external_flash' serves two purposes:
 * - By default, the compiler is free to inline any function call he wants. If
 *   the compiler decides to inline some functions that make use of VFP
 *   registers, it will need to push VFP them onto the stack in calling
 *   function's prologue.
 *   Problem: in start()'s prologue, we would never had a chance to enable the
 *   FPU since this function is the first thing called after reset.
 *   We can safely assume that neither memcpy, memset, nor any Ion::Device::init*
 *   method will use floating-point numbers, but ion_main very well can.
 *   To make sure ion_main's potential usage of VFP registers doesn't bubble-up to
 *   start(), we isolate it in its very own non-inlined function call.
 * - To avoid jumping on the external flash when it is shut down, we ensure
 *   there is no symbol references from the internal flash to the external
 *   flash except this jump. In order to do that, we isolate this
 *   jump in a symbol that we link in a special section separated from the
 *   internal flash section. We can than forbid cross references from the
 *   internal flash to the external flash. */

typedef void (*ExternalStartPointer)();

static void __attribute__((noinline)) jump_to_external_flash() {
  Ion::Device::Board::initPeripherals(false);

  bool authenticated = ExternalFlashIsAuthenticated();
  /* Init the peripherals. We do not initialize the backlight in case there is
   * an on boarding app: indeed, we don't want the user to see the LCD tests
   * happening during the on boarding app. The backlight will be initialized
   * after the Power-On Self-Test if there is one or before switching to the
   * home app otherwise. */

  /* Re-configurate the MPU to forbid access to blue LED if required */
  if (!authenticated) {
    // TODO EMILIE
    // Shutdown the LED (blue and green)
    Ion::Device::Regs::AFGPIOPin(Ion::Device::Regs::GPIOB, 0,  Ion::Device::Regs::GPIO::AFR::AlternateFunction::AF2, Ion::Device::Regs::GPIO::PUPDR::Pull::None, Ion::Device::Regs::GPIO::OSPEEDR::OutputSpeed::Low).shutdown();
    Ion::Device::Regs::AFGPIOPin(Ion::Device::Regs::GPIOB, 5,  Ion::Device::Regs::GPIO::AFR::AlternateFunction::AF2, Ion::Device::Regs::GPIO::PUPDR::Pull::None, Ion::Device::Regs::GPIO::OSPEEDR::OutputSpeed::Low).shutdown();
    // MPU on Blue LED
    Ion::Device::Cache::dmb();
    Ion::Device::Regs::MPU.RNR()->setREGION(7);
    /* We want to forbid access to the 2 first 32-bit registers of GPIOB (MODER &
     * TYPER) but the smallest MPU region size is 32-byte long. So we offset the
     * MPU region by 6 bytes - nothing seems to there? */
    Ion::Device::Regs::MPU.RBAR()->setADDR(0x40020400-6);
    Ion::Device::Regs::MPU.RASR()->setSIZE(Ion::Device::Regs::MPU::RASR::RegionSize::_32B);
    Ion::Device::Regs::MPU.RASR()->setAP(Ion::Device::Regs::MPU::RASR::AccessPermission::NoAccess);
    Ion::Device::Regs::MPU.RASR()->setXN(false);
    Ion::Device::Regs::MPU.RASR()->setTEX(2);
    Ion::Device::Regs::MPU.RASR()->setS(0);
    Ion::Device::Regs::MPU.RASR()->setC(0);
    Ion::Device::Regs::MPU.RASR()->setB(0);
    Ion::Device::Regs::MPU.RASR()->setENABLE(true);

    // INTERNAL FLASH //TODO EMILIE true comment?
    Ion::Device::Regs::MPU.RNR()->setREGION(6);
    Ion::Device::Regs::MPU.RBAR()->setADDR(0x40023C00);
    Ion::Device::Regs::MPU.RASR()->setSIZE(Ion::Device::Regs::MPU::RASR::RegionSize::_32B);
    Ion::Device::Regs::MPU.RASR()->setAP(Ion::Device::Regs::MPU::RASR::AccessPermission::NoAccess);
    Ion::Device::Regs::MPU.RASR()->setXN(false);
    Ion::Device::Regs::MPU.RASR()->setTEX(2);
    Ion::Device::Regs::MPU.RASR()->setS(0);
    Ion::Device::Regs::MPU.RASR()->setC(0);
    Ion::Device::Regs::MPU.RASR()->setB(0);
    Ion::Device::Regs::MPU.RASR()->setENABLE(true);

    Ion::Device::Cache::dsb();
    Ion::Device::Cache::isb();

    Ion::Device::Backlight::init();
    displayWarningMessage();
  }

  //ColorScreen(0x00FF00);

  /* Unprivileged mode */
  switch_to_unpriviledged();
  Ion::Device::Cache::isb();

  ExternalStartPointer * externalFlashFirstAddress = reinterpret_cast<ExternalStartPointer *>(0x9000D028); // TODO EMILIE explain + magic number
  ExternalStartPointer external_flash_entry = *externalFlashFirstAddress;
  external_flash_entry();
}

/* When 'start' is executed, the external flash is supposed to be shutdown. We
 * thus forbid inlining to prevent executing this code from external flash
 * (just in case 'start' was to be called from the external flash). */

void __attribute__((noinline)) start() {
  /* This is where execution starts after reset.
   * Many things are not initialized yet so the code here has to pay attention. */

  /* Copy data section to RAM
   * The data section is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data section (e.g. global variables) contains
   * garbage values and should not be used. */
  size_t dataSectionLength = (&_data_section_end_ram - &_data_section_start_ram);
  memcpy(&_data_section_start_ram, &_data_section_start_flash, dataSectionLength);

  /* Zero-out the bss section in RAM
   * Until we do, any uninitialized global variable will be unusable. */
  size_t bssSectionLength = (&_bss_section_end_ram - &_bss_section_start_ram);
  memset(&_bss_section_start_ram, 0, bssSectionLength);

  /* Initialize the FPU as early as possible.
   * For example, static C++ objects are very likely to manipulate float values */
  Ion::Device::Board::initFPU();

  /* Call static C++ object constructors
   * The C++ compiler creates an initialization function for each static object.
   * The linker then stores the address of each of those functions consecutively
   * between _init_array_start and _init_array_end. So to initialize all C++
   * static objects we just have to iterate between theses two addresses and
   * call the pointed function. */
#define SUPPORT_CPP_GLOBAL_CONSTRUCTORS 0
#if SUPPORT_CPP_GLOBAL_CONSTRUCTORS
  for (cxx_constructor * c = &_init_array_start; c<&_init_array_end; c++) {
    (*c)();
  }
#else
  /* In practice, static initialized objects are a terrible idea. Since the init
   * order is not specified, most often than not this yields the dreaded static
   * init order fiasco. How about bypassing the issue altogether? */
  if (&_init_array_start != &_init_array_end) {
    abort();
  }
#endif

  Ion::Device::Board::init();

  /* At this point, we initialized clocks and the external flash but no other
   * peripherals. */

  jump_to_external_flash();

  abort();
}

void __attribute__((interrupt, noinline)) isr_systick() {
  auto t = Ion::Device::Timing::MillisElapsed;
  t++;
  Ion::Device::Timing::MillisElapsed = t;
}

void displayInterruptionMessage() {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(screen, KDColorGreen);
  for (volatile uint32_t i=0; i<38*1000; i++) {
    __asm volatile("nop");
  }
  Ion::Display::pushRectUniform(screen, KDColorWhite);
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  KDCoordinate margin = 60;
  KDCoordinate currentHeight = 0;
  currentHeight += margin;
  const char * title = "I got Interrupted by EXTI!";
  KDSize titleSize = KDFont::LargeFont->stringSize(title);
  ctx->drawString(title, KDPoint((Ion::Display::Width-titleSize.width())/2, currentHeight), KDFont::LargeFont);
}

using namespace Ion::Device::Regs;

void __attribute__((interrupt, noinline)) keyboard_handler() {
  for (uint8_t i=0; i < Ion::Device::Keyboard::Config::numberOfColumns; i++) {
    uint8_t pin = Ion::Device::Keyboard::Config::RowPins[i];
    if (Ion::Device::Regs::EXTI.PR()->get(pin)) {
      Ion::Device::Regs::EXTI.PR()->set(pin, true);
      displayInterruptionMessage();
    }
  }
}
