#include "../../../shared/platform.h"
#include <jni.h>
#include <assert.h>

namespace Ion {
namespace Simulator {
namespace Platform {

const char * languageCode() {
  static char buffer[4] = {0};
  if (buffer[0] == 0) {
    JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

    jclass j_class = env->FindClass("io/github/omega/simulator/OmegaActivity");
    jmethodID j_methodId = env->GetMethodID(
      j_class,
      "retrieveLanguage",
      "()Ljava/lang/String;"
    );
    assert(j_methodId != 0);

    jstring j_language = static_cast<jstring>(env->CallObjectMethod(activity, j_methodId));
    const char * language = env->GetStringUTFChars(j_language, nullptr);
    memcpy(buffer, language, 4);
    buffer[3] = 0;
    env->ReleaseStringUTFChars(j_language, language);
    env->DeleteLocalRef(j_language);
    env->DeleteLocalRef(j_class);
    env->DeleteLocalRef(activity);
  }
  return buffer;
}

}
}
}
