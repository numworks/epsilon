#pragma once

#ifdef __cplusplus
extern "C" {
#endif

const char* IonSoftwareVersion();
const char* IonPatchLevel();
void IonSimulatorKeyboardKeyDown(int keyIndex);
void IonSimulatorKeyboardKeyUp(int keyIndex);
void IonSimulatorEventsPushEvent(int eventNumber);

#ifdef __cplusplus
}
#endif
