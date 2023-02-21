#include <SDL.h>
#include <assert.h>
#include <jni.h>

#include "../../../shared/haptics.h"

namespace Ion {
namespace Simulator {
namespace Haptics {

bool isEnabled() {
  JNIEnv *env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId =
      env->GetMethodID(j_class, "hapticFeedbackIsEnabled", "()Z");
  assert(j_methodId != 0);
  bool result = (env->CallBooleanMethod(activity, j_methodId) != JNI_FALSE);
  /* Local references are automatically deleted if a native function called from
   * Java side returns. For SDL this native function is main() itself. Therefore
   * references need to be manually deleted because otherwise they are uselessly
   * accumulated each time isEnabled is called, which happens repetitively
   * before the application exit. */
  env->DeleteLocalRef(j_class);
  env->DeleteLocalRef(activity);
  return result;
}

}  // namespace Haptics
}  // namespace Simulator
}  // namespace Ion
