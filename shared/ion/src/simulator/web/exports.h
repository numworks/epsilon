#pragma once

#ifdef __cplusplus
extern "C" {
#endif

const char* IonSoftwareVersion();
const char* IonPatchLevel();
void IonSimulatorKeyboardKeyDown(int keyNumber);
void IonSimulatorKeyboardKeyUp(int keyNumber);
void IonSimulatorEventsPushEvent(int eventNumber);

#ifdef __cplusplus
}
#endif
