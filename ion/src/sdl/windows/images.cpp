#include <SDL.h>
#include <windows.h>
#include <olectl.h>
#include <assert.h>
#include <gdiplus.h>
#include <stdio.h>

HRESULT CreateStreamOnResource(const char * name, LPSTREAM * stream) {
  assert(name != nullptr);
  assert(stream != nullptr);
  HINSTANCE hInstance = GetModuleHandle(0);
  *stream = NULL;
  HRSRC hC = FindResource(hInstance, name, RT_RCDATA);
  if (!hC) {
    SDL_Log("Could not find resource %s", name);
    return E_INVALIDARG;
  }
    // This is not really a HGLOBAL http://msdn.microsoft.com/en-us/library/windows/desktop/ms648046(v=vs.85).aspx
  HGLOBAL hG = LoadResource(hInstance, hC);
  if (!hG) {
    SDL_Log("Could not load resource %s", name);
    return E_INVALIDARG;
  }
  void* bytes = LockResource(hG);
  ULONG size = SizeofResource(hInstance, hC);
  // Create a new empty stream.
  HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, stream);
  if (SUCCEEDED(hr)) {
    ULONG written;
    // Copy the resource into it.
    hr = (*stream)->Write(bytes, size, &written);
  }
  return hr;
}

extern "C" SDL_Texture * loadImage(SDL_Renderer * renderer, const char * identifier) {
//SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Loading image");
  SDL_Log("chabite");

  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

  LPSTREAM stream;
  const char * resname = MAKEINTRESOURCE(300);
  CreateStreamOnResource(resname, &stream);
#if 0
  IPicture * picture = nullptr;
  OleLoadPicture(
      stream,
      0,
      false,
      IID_IPicture,
      (void**)&picture
      );
#endif

  Gdiplus::Bitmap * image = Gdiplus::Bitmap::FromStream(stream);

  SDL_Log("Gdiplus::Bitmap is %p", image);

  int w = (int)image->GetWidth();
  int h = (int)image->GetHeight();
  Gdiplus::Rect rc(0, 0, w, h);

  SDL_Log("Gdiplus::Bitmap is %dx%d", w,h);

  Gdiplus::BitmapData * bitmapData = new Gdiplus::BitmapData;
  image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

  SDL_Texture * texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC,
      w,
      h
      );

  SDL_UpdateTexture(
      texture,
      NULL,
      bitmapData->Scan0,
      4 * w
      );

  image->UnlockBits(bitmapData);
  delete bitmapData;
  delete image;
  Gdiplus::GdiplusShutdown(gdiplusToken);

  return texture;
}
