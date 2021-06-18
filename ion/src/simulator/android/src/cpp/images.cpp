#include "../../../shared/platform.h"
#include <SDL.h>
#include <jni.h>
#include <android/bitmap.h>

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier) {
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

  jclass j_class = env->FindClass("io/github/omega/simulator/OmegaActivity");
  jmethodID j_methodId = env->GetMethodID(
    j_class,
    "retrieveBitmapAsset",
    "(Ljava/lang/String;)Landroid/graphics/Bitmap;"
  );

  jstring j_identifier = env->NewStringUTF(identifier);

  jobject j_bitmap = env->CallObjectMethod(activity, j_methodId, j_identifier);

  AndroidBitmapInfo bitmapInfo;
  AndroidBitmap_getInfo(env, j_bitmap, &bitmapInfo);

  void * bitmapPixels = nullptr;
  AndroidBitmap_lockPixels(env, j_bitmap, &bitmapPixels);
  // TODO: Handle the case where lockPixels fails

  size_t bytesPerPixel = 4;

  SDL_Texture * texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STATIC,
    bitmapInfo.width,
    bitmapInfo.height
  );

  SDL_UpdateTexture(
      texture,
      nullptr,
      bitmapPixels,
      bytesPerPixel * bitmapInfo.width
  );

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  AndroidBitmap_unlockPixels(env, j_bitmap);

  return texture;
}
