#include "../../../shared/platform.h"
#include <SDL.h>
#include <jni.h>
#include <android/bitmap.h>

SDL_Surface * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier) {
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
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
  size_t bitsPerPixel = bytesPerPixel*8;
  SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormatFrom(
    bitmapPixels,
    bitmapInfo.width,
    bitmapInfo.height,
    bitsPerPixel,
    bytesPerPixel * bitmapInfo.width,
    SDL_PIXELFORMAT_ABGR8888);

  AndroidBitmap_unlockPixels(env, j_bitmap);

  return surface;
}
