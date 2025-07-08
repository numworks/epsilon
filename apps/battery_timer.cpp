#include "battery_timer.h"

#include <ion/usb.h>

#include "apps_container.h"

constexpr static I18n::Message k_chargingPromptMessages[] = {
    I18n::Message::BatteryChargingTitle, I18n::Message::BlankMessage,
    I18n::Message::BatteryChargingMessage,
    I18n::Message::BatteryChargingMessage2};
constexpr static KDColor k_chargingPromptColors[] = {
    KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack};
constexpr static uint8_t k_chargingPromptNumberOfMessages = 4;

bool dismissWhenUnplugged(Ion::Events::Event) {
  if (!Ion::USB::isPlugged()) {
    Escher::App::app()->modalViewController()->dismissModal();
    return true;
  }
  return false;
}

BatteryTimer::BatteryTimer()
    : Timer(1),
      m_chargingPromptController(
          k_chargingPromptMessages, k_chargingPromptColors,
          k_chargingPromptNumberOfMessages, dismissWhenUnplugged) {}

bool BatteryTimer::fire() {
  AppsContainer* container = AppsContainer::sharedAppsContainer();
  bool needRedrawing = container->updateBatteryState();
  bool isPlugged = Ion::USB::isPlugged();
  if (Ion::Battery::level() == Ion::Battery::Charge::EMPTY && !isPlugged) {
    container->shutdownDueToLowBattery();
  }
  if (!isPlugged) {
    m_showModalTickCountdown = k_numberOfPluggedFireBeforeShowModal;
  } else if (m_showModalTickCountdown > 0) {
    --m_showModalTickCountdown;
  } else if (m_showModalTickCountdown == 0) {
    Escher::App::app()->displayModalViewController(&m_chargingPromptController,
                                                   0.f, 0.f);
    m_showModalTickCountdown = k_alreadyShownModal;
    needRedrawing = true;
  }
  return needRedrawing;
}
