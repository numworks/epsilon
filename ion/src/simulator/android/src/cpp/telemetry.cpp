#include "../../../shared/platform.h"
#include <jni.h>
#include <SDL.h>

void IonSDLPlatformTelemetryInit() {
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(j_class,"telemetryInit", "()V");

  env->CallVoidMethod(activity, j_methodId);
}

void IonSDLPlatformTelemetryEvent(const char * eventName) {
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(j_class,"telemetryEvent", "(Ljava/lang/String;)V");

  jstring j_eventName = env->NewStringUTF(eventName);

  env->CallVoidMethod(activity, j_methodId, j_eventName);
}

void IonSDLPlatformTelemetryDeinit() {
}
