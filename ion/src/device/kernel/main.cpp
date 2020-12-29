#include <kernel/drivers/config/board.h>
#include <drivers/cache.h>
#include <kernel/drivers/backlight.h>
#include <kernel/drivers/display.h>
#include <kernel/drivers/led.h> // TODO EMILIE: remove
#include <kernel/drivers/timing.h>
#include <ion/display.h>
#include <kandinsky.h>

extern "C" {
  extern void switch_to_unpriviledged();
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
  Ion::Device::Display::pushRectUniform(screen, KDColorWhite);
  // TODO EMILIE untangle this from kandinsky -->
  // in kernel build font and make an API to access font!
  /*KDContext * ctx = KDIonContext::sharedContext();
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
  }*/
  Ion::Device::Timing::msleep(5000);
}
typedef void (*StartPointer)();

void kernel_main(bool numworksAuthentication) {
  if (!numworksAuthentication) {
    Ion::Device::Backlight::init();
    displayWarningMessage();
    Ion::Device::Backlight::shutdown();
  }
  /* Unprivileged mode */
  KDColor c = KDColorWhite;
  Ion::Device::LED::setColor(c); // TODO: remove me

  while (1) {
    Ion::Keyboard::State sLastKeyboardState;
    Ion::Keyboard::State state = Ion::Device::Keyboard::Queue::sharedQueue()->isEmpty() ? sLastKeyboardState : Ion::Device::Keyboard::Queue::sharedQueue()->pop();
    if (sLastKeyboardState != state) {
      if (c == KDColorWhite) {
        c = KDColorBlack;
        Ion::Device::LED::setColor(c);
      } else {
        c = KDColorWhite;
        Ion::Device::LED::setColor(c);
      }
    }
  }
  switch_to_unpriviledged();
  Ion::Device::Cache::isb();


  /* Jump to userland */
  StartPointer * userlandFirstAddress = reinterpret_cast<StartPointer *>(Ion::Device::Board::Config::UserlandAddress);
  StartPointer userlandEntry = *userlandFirstAddress;
  userlandEntry();
}
