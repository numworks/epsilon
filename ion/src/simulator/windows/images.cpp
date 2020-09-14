#include "../shared/platform.h"
#include <ion/src/simulator/windows/resources.h>

#include <SDL.h>
#include <windows.h>
#include <olectl.h>
#include <gdiplus.h>
#include <assert.h>

/* Loading images using GDI+
 * On Windows, we decompress JPEG images using GDI+ which is widely available.
 * Note that this adds an extra runtime dependency (as compared to just SDL),
 * but this should not be an issue. */

HRESULT CreateStreamOnResource(const char * name, LPSTREAM * stream) {
  HINSTANCE hInstance = GetModuleHandle(0);
  *stream = nullptr;
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
  void * bytes = LockResource(hG);
  ULONG size = SizeofResource(hInstance, hC);
  HRESULT hr = CreateStreamOnHGlobal(NULL, true, stream);
  if (SUCCEEDED(hr)) {
    ULONG written;
    hr = (*stream)->Write(bytes, size, &written);
  }
  return hr;
}

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier, bool withTransparency, uint8_t alpha) {
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

  LPSTREAM stream;
  int resourceID = -1;
  for (size_t i = 0; i < sizeof(resourcesIdentifiers)/sizeof(resourcesIdentifiers[0]); i++) {
    if (strcmp(identifier, resourcesIdentifiers[i].identifier) == 0) {
      resourceID = resourcesIdentifiers[i].id;
    }
  }
  assert(resourceID >= 0);
  const char * resname = MAKEINTRESOURCE(resourceID);
  CreateStreamOnResource(resname, &stream);

  Gdiplus::Bitmap * image = Gdiplus::Bitmap::FromStream(stream);

  int width = (int)image->GetWidth();
  int height = (int)image->GetHeight();
  Gdiplus::Rect rc(0, 0, width, height);

  Gdiplus::BitmapData * bitmapData = new Gdiplus::BitmapData;
  image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

  size_t bytesPerPixel = 4;
  size_t bitsPerPixel = bytesPerPixel*8;
  SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormatFrom(
    bitmapData->Scan0,
    width,
    height,
    bitsPerPixel,
    bytesPerPixel*width,
    SDL_PIXELFORMAT_ABGR8888);

  SDL_SetColorKey(surface, withTransparency, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
  SDL_SetSurfaceAlphaMod(surface, alpha);

  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

  image->UnlockBits(bitmapData);
  delete bitmapData;
  delete image;
  Gdiplus::GdiplusShutdown(gdiplusToken);
  SDL_FreeSurface(surface);

  return texture;
}
