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
  // TODO : GetMethodID is relatively costly, and could be performed only once.
  jmethodID j_methodId = env->GetMethodID(j_class,"hapticFeedbackIsEnabled", "()Z");
  if (j_methodId == 0) {
    return false;
  }
  return (env->CallBooleanMethod(activity, j_methodId) != JNI_FALSE);
}

}
}
}
