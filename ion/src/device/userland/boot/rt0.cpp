#include "entry_point.h"
#include <shared/boot/rt0.h>
#include <ion.h>
#include <ion/backlight.h> // TODO: remove
#include <ion/console.h> // TODO: remove
#include <ion/display.h> // TODO: remove
#include <ion/persisting_bytes.h> // TODO: remove
#include <ion/usb.h> // TODO: remove
#include <ion/events.h> // TODO: remove

extern "C" {
  void abort();
}

void abort() { //TODO EMILIE: expose in ion API
#ifdef NDEBUG
  //svc(SVC_RESET_CORE);
#else
  while (1) {
  }
#endif
}

void ion_main(int argc, const char * const argv[]) {
  Ion::Backlight::init();
  KDRect rect(0,0, 199, 100);
  KDColor c = KDColorRed;
  uint8_t e = Ion::PersistingBytes::read();
  if (e % 2) {
    c = KDColorGreen;
  }
  Ion::Display::pushRectUniform(rect, c);
  KDRect r(195,0, 10, 10);
  KDColor pixels[100];
  Ion::Display::pullRect(r, pixels);
  Ion::Display::pushRect(KDRect(0,0,10,10), pixels);

#if 0
  Ion::USB::enable();
  while (!Ion::USB::isEnumerated()) {
    for (volatile uint32_t i=0; i<10000; i++) {
      __asm volatile("nop");
    }
  }
  Ion::USB::DFU();
  Ion::USB::disable();
#endif

  c = KDColorBlue;
  const char * patch = Ion::patchLevel();
  const char * fcc = Ion::fccId();
  while (1) {
    int timeout = 300;
    //Ion::Events::Event e = Ion::Events::getEvent(&timeout);
    Ion::Keyboard::State s = Ion::Keyboard::scan();
    if (s.keyDown(Ion::Keyboard::Key::Shift)) {
      uint32_t data[2] = {0xAABBEEFF, 0xCCDD0011};
      Ion::crc32Word(data, 2);
      Ion::crc32Byte(reinterpret_cast<uint8_t *>(data), 2*4);
      Ion::Display::pushRectUniform(KDRect(100,100,20,20), KDColorRed);
      Ion::LED::setColor(KDColorRed);
    }
#if 0
    if (e != Ion::Events::None) {
      Ion::Display::pushRectUniform(KDRect(0,0,100,100), c);
      //Ion::Display::displayColoredTilingSize10();
      Ion::PersistingBytes::write(Ion::PersistingBytes::read() + 1);
      c = c == KDColorBlue ? KDColorGreen : KDColorBlue;
    }
    if (e == Ion::Events::OK) {
      Ion::Console::writeChar('a');
      /*uint8_t b = Ion::Backlight::brightness();
      b = (b + 240/12) % 240;
      Ion::Backlight::setBrightness(12);*/
    }
    if (e == Ion::Events::OnOff) {
      //Ion::Power::standby();
      Ion::Power::suspend(true);
      /*uint8_t b = Ion::Backlight::brightness();
      b = (b + 240/12) % 240;
      Ion::Backlight::setBrightness(12);*/
    }
    if (e == Ion::Events::Home) {
      if (Ion::Backlight::isInitialized()) {
        Ion::Backlight::shutdown();
      } else {
        Ion::Backlight::init();
      }
    }
#endif
    if (Ion::Battery::isCharging()) {
      Ion::Display::pushRectUniform(KDRect(0,40,100,100), KDColorWhite);
    } else {
      Ion::Display::pushRectUniform(KDRect(0,40,100,100), KDColorBlack);
    }
    /*for (volatile uint32_t i=0; i<10000; i++) {
      __asm volatile("nop");
    }*/
  }
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  ion_main(0, nullptr);
  abort();
}

