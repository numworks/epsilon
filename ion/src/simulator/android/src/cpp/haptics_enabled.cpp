#include "../../../shared/haptics.h"
#include <jni.h>
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Haptics {

bool isEnabled() {
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(j_class,"hapticFeedbackIsEnabled", "()Z");

  return env->CallObjectMethod(activity, j_methodId);
}

}
}
}
