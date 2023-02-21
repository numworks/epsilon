#include "../../../shared/telemetry.h"

#include <SDL.h>
#include <assert.h>
#include <jni.h>

static inline JNIEnv *AndroidJNI() {
  return static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
}

static inline jobject AndroidActivity() {
  return static_cast<jobject>(SDL_AndroidGetActivity());
}

static inline jstring JS(const char *s, JNIEnv *env) {
  return env->NewStringUTF(s);
}

namespace Ion {
namespace Simulator {
namespace Telemetry {

void init() {
  JNIEnv *env = AndroidJNI();

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(j_class, "telemetryInit", "()V");
  assert(j_methodId != 0);

  env->CallVoidMethod(AndroidActivity(), j_methodId);
  env->DeleteLocalRef(j_class);
}

void shutdown() {}

}  // namespace Telemetry
}  // namespace Simulator
}  // namespace Ion

namespace Ion {
namespace Telemetry {

void reportScreen(const char *screenName) {
  JNIEnv *env = AndroidJNI();

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId =
      env->GetMethodID(j_class, "telemetryScreen", "(Ljava/lang/String;)V");
  assert(j_methodId != 0);

  env->CallVoidMethod(AndroidActivity(), j_methodId, JS(screenName, env));
  env->DeleteLocalRef(j_class);
}

void reportEvent(const char *category, const char *action, const char *label) {
  JNIEnv *env = AndroidJNI();

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(
      j_class, "telemetryEvent",
      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
  assert(j_methodId != 0);

  env->CallVoidMethod(AndroidActivity(), j_methodId, JS(category, env),
                      JS(action, env), JS(label, env));
  env->DeleteLocalRef(j_class);
}

}  // namespace Telemetry
}  // namespace Ion
