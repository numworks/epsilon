#ifndef ION_SIMULATOR_WEB_EXPORTS_H
#define ION_SIMULATOR_WEB_EXPORTS_H

#ifdef __cplusplus
extern "C" {
#endif

void IonSimulatorKeyboardKeyDown(int keyNumber);
void IonSimulatorKeyboardKeyUp(int keyNumber);
void IonSimulatorEventsPushEvent(int eventNumber);

#ifdef __cplusplus
}
#endif

#endif
