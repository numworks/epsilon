#ifndef ION_DEVICE_KERNEL_EVENTS_H
#define ION_DEVICE_KERNEL_EVENTS_H

#include <ion/events.h>
#include <kandinsky/rect.h>

namespace Ion {
namespace Device {
namespace Events {

void init();
void shutdown();
void initInterruptions();
void shutdownInterruptions();

Ion::Events::Event getEvent(int * timeout);
Ion::Events::Event getPlatformEvent();

/* The functions copyTextSecure and isDefinedSecure could have been methods of
 * the class 'Event' but, since they're used as SVCallHandlers, it would be
 * unsafe to rely on the fact that these methods are'n't in the vtable and thus
 * don't jump to an address specified at run time. That would make the code
 * quite vulnerable: by forging an event with a custom vtable, a attacker could
 * execute some code with kernel privilege. */
size_t copyTextSecure(uint8_t eventId, char * buffer, size_t bufferSize);
bool isDefinedSecure(uint8_t eventId);

void pauseStallingTimer();
void resetStallingTimer();
void stall();
constexpr int k_spinnerHalfHeight = 4;
constexpr int k_spinnerHalfWidth = 8;
constexpr int k_barHeight = 18; // Escher::Metric::TitleBarHeight
constexpr int k_spinnerX = 320 - 5 - 15; // Ion::Display::Width - Escher::Metric::TitleBarExternHorizontalMargin - BatteryView::k_batteryWidth
constexpr int k_spinnerY = k_barHeight/2 - k_spinnerHalfHeight;
void setSpinner(bool spinner);
void spin();
void hideSpinner();

}
}
}

#endif
