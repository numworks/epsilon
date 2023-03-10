extern "C" {
#include "../include/eadk/eadk.h"
}
#include <ion.h>
#include <ion/src/shared/events.h>
#include <kandinsky/ion_context.h>

#include <cstdlib>

static inline void preemptive_termination(Ion::Events::Event e) {
  if (e == Ion::Events::Home || e == Ion::Events::Termination) {
    exit(0);
  }
}

eadk_keyboard_state_t eadk_keyboard_scan() {
  /* Calling Ion::Events::getPlatformEvent gives us a change to handle simulator
   * events, like "take a screenshot", or "us the mouse to press a button on the
   * virtual calculator". */
  Ion::Events::Event e = Ion::Events::getPlatformEvent();
  preemptive_termination(e);
  return Ion::Keyboard::scan();
}

eadk_event_t eadk_event_get(int32_t *timeout) {
  Ion::Events::Event e = Ion::Events::getEvent(timeout);
  preemptive_termination(e);
  return (uint8_t)e;
}

void eadk_backlight_set_brightness(uint8_t brightness) {
  Ion::Backlight::setBrightness(brightness);
}

uint8_t eadk_backlight_brightness() { return Ion::Backlight::brightness(); }

bool eadk_battery_is_charging() { return Ion::Battery::isCharging(); }

uint8_t eadk_battery_level() { return (uint8_t)Ion::Battery::level(); }

float eadk_battery_voltage() { return Ion::Battery::voltage(); }

static_assert(sizeof(KDColor) == sizeof(eadk_color_t), "Size mismatch");
static inline KDColor c(eadk_color_t color) {
  return *reinterpret_cast<KDColor *>(&color);
}

static_assert(sizeof(KDPoint) == sizeof(eadk_point_t), "Size mismatch");
static inline KDPoint p(eadk_point_t point) {
  return *reinterpret_cast<KDPoint *>(&point);
}

static_assert(sizeof(KDRect) == sizeof(eadk_rect_t), "Size mismatch");
static inline KDRect r(eadk_rect_t rect) {
  return *reinterpret_cast<KDRect *>(&rect);
}

void eadk_display_push_rect(eadk_rect_t rect, const eadk_color_t *pixels) {
  Ion::Display::pushRect(r(rect), reinterpret_cast<const KDColor *>(pixels));
}
void eadk_display_push_rect_uniform(eadk_rect_t rect, eadk_color_t color) {
  Ion::Display::pushRectUniform(r(rect), c(color));
}

void eadk_display_pull_rect(eadk_rect_t rect, eadk_color_t *pixels) {
  Ion::Display::pullRect(r(rect), reinterpret_cast<KDColor *>(pixels));
}

bool eadk_display_wait_for_vblank() { return Ion::Display::waitForVBlank(); }

void eadk_display_draw_string(const char *text, eadk_point_t point,
                              bool large_font, eadk_color_t text_color,
                              eadk_color_t background_color) {
  // TODO: Duplicated from ion/src/device/userland/drivers/display.cpp
  KDContext *ctx = KDIonContext::SharedContext;
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(KDRectScreen);
  ctx->drawString(text, p(point),
                  KDGlyph::Style{.glyphColor = c(text_color),
                                 .backgroundColor = c(background_color),
                                 .font = large_font ? KDFont::Size::Large
                                                    : KDFont::Size::Small},
                  255);
}

void eadk_timing_usleep(uint32_t us) {
  // Ion::Timing::usleep(us); // TODO
}

void eadk_timing_msleep(uint32_t ms) { Ion::Timing::msleep(ms); }

uint64_t eadk_timing_millis() { return Ion::Timing::millis(); }

const char *eadk_external_data = nullptr;
extern size_t eadk_external_data_size = 0;

bool eadk_usb_is_plugged() { return Ion::USB::isPlugged(); }

uint32_t eadk_random() { return Ion::random(); }
