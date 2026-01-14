#pragma once

#ifdef __cplusplus
extern "C" {
#endif

const char* IonSoftwareVersion();
const char* IonPatchLevel();
void IonSimulatorKeyboardKeyDown(int keyNumber);
void IonSimulatorKeyboardKeyUp(int keyNumber);
void IonSimulatorEventsPushEvent(int eventNumber);
void IonSimulatorEventsPushEventArray(const char* eventBufferAsChar, int length,
                                      int fromIndex, int toIndex);
int IonSimulatorNumberOfEventsInArray(const char* eventBuffer, int length);

#ifdef __cplusplus
}
#endif
