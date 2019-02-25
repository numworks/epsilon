// This shall be implemented per-platform
//
#include <SDL.h>
#include <jni.h>
#include <android/bitmap.h>

SDL_Surface * loadImage(const char * identifier) {
  JNIEnv * env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

  jstring j_identifier = env->NewStringUTF(identifier);

  jclass j_class = env->FindClass("com/numworks/calculator/EpsilonActivity");
  jmethodID j_methodId = env->GetMethodID(
    j_class,
    "retrieveBitmapAsset",
    "(Ljava/lang/String;)Landroid/graphics/Bitmap;"
  );

  jobject j_bitmap = env->CallObjectMethod(activity, j_methodId, j_identifier);

  AndroidBitmapInfo bitmapInfo;
  AndroidBitmap_getInfo(env, j_bitmap, &bitmapInfo);

  void * bitmapPixels = nullptr;
  AndroidBitmap_lockPixels(env, j_bitmap, &bitmapPixels);
  // TODO: Handle the case where lockPixels fails

  SDL_Surface * inputSurface = SDL_CreateRGBSurfaceWithFormatFrom(
    bitmapPixels,
    bitmapInfo.width,
    bitmapInfo.height,
    32, // BPP. TODO: Infer from pixel format
    4 * bitmapInfo.width, // Pitch. TODO: Infer from pixel format
    SDL_PIXELFORMAT_ABGR8888
  );

  SDL_Surface * outputSurface = SDL_CreateRGBSurface(
    0, // Flags. Unused.
    bitmapInfo.width, // Width
    bitmapInfo.height, // Height
    32, // Bits per pixel
    0, 0, 0, 0 // Default masks for the given depth
  );

  SDL_BlitSurface(inputSurface, NULL, outputSurface, NULL);

  SDL_FreeSurface(inputSurface);

  AndroidBitmap_unlockPixels(env, j_bitmap);

  return outputSurface;
}
