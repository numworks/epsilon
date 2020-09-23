#include "../../../shared/haptics.h"
#include <jni.h>
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Haptics {

static SDL_Haptic * sSDLHaptic = nullptr;

void init() {
  if (SDL_Init(SDL_INIT_HAPTIC) == 0) {
    sSDLHaptic = SDL_HapticOpen(0);
    if (sSDLHaptic) {
      if (SDL_HapticRumbleInit(sSDLHaptic) != 0) {
        sSDLHaptic = nullptr;
      }
    }
  }
}

void shutdown() {
  if (sSDLHaptic) {
    SDL_HapticClose(sSDLHaptic);
  }
}

void perform() {
  // Retrieve system setting: is haptic feedback enabled?
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(j_class,"hapticFeedbackIsEnabled", "()Z");

  bool feedbackEnabled = env->CallObjectMethod(activity, j_methodId);

  if (feedbackEnabled && sSDLHaptic) {
    SDL_HapticRumblePlay(sSDLHaptic, 1.0, 40);
  }
}


}
}
}

