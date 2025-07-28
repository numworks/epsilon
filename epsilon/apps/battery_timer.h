#ifndef APPS_BATTERY_TIMER_H
#define APPS_BATTERY_TIMER_H

#include <apps/shared/prompt_controller.h>
#include <escher/timer.h>

class BatteryTimer : public Escher::Timer {
 public:
  BatteryTimer();

  void doNotShowModal() { m_showModalTickCountdown = k_alreadyShownModal; }

 private:
  bool fire() override;
  Shared::PromptController m_chargingPromptController;

  /* Waiting around 1 second of charging time to display modal, this almost
   * ensures the USB modal shows before if the calculator is connected to a PC
   */
  constexpr static int8_t k_numberOfPluggedFireBeforeShowModal = 3;
  constexpr static int8_t k_alreadyShownModal = -1;
  int8_t m_showModalTickCountdown = k_alreadyShownModal;
};

#endif
